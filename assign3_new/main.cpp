#include <cmath>
#include <list>
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;



class fileOrDir {
  public:
    string name;
    string timeStamp;
    bool isDirectory; // either a regular file or a directory
    long fileSize;
    int allocatedBytes;
    // Linked List Lfile for Storing Disk Block Addresses for A File
    list<int> blockAddresses;
};

class diskBlock {
  public:
    int start;
    int end;
    bool isFree;
};


class treeNode {
  public:
    fileOrDir *data;
    vector<treeNode*> children;
    treeNode *parent;
};

// global variables
list<diskBlock*> diskBlocks;
treeNode *root = new treeNode;
treeNode *currentDir = new treeNode;

string rebuildTerminalPath(vector<string> vec){
    string returnVal = "";
    cout << "REBUILDING STRINGS " << endl;
    for(int x = 0; x<vec.size(); x++){
        cout << vec[x] << endl;
        returnVal += vec[x];
    }
    return returnVal;
}

int calculateBytesUnused(int fileSize, int blockSize) {
    if (fileSize == 0) {
        return 0;
    }
    int blocks = ceil((float)fileSize / (float)blockSize);
    if (blocks == fileSize / blockSize) {
        return 0;
    }
    return blockSize - (fileSize - ((blocks - 1) * blockSize));
}


// Print out directory tree in breadth-first order
void printDirectory(treeNode *root) {
    if (root != NULL) {
        treeNode *temp = root;
        list<treeNode *> queue;
        queue.push_back(root);
        while(!queue.empty()){
            // print the current directory name
            temp = queue.front();
            cout << temp->data->name << endl;
            queue.pop_front();
            for (int i = 0; i < temp->children.size(); ++i) {
                queue.push_back(temp -> children[i]);
            }
        }
    }
}

//prints out sub-directories under a given node
void printDirChildren(treeNode *node) {
    for (int i = 0; i < (node->children).size(); ++i)
    {
        if(node->children[i]->data->isDirectory == true){
            cout << node->children[i]->data->name << endl;
        }
    }
    cout << endl;
}

// BST
treeNode* findNode(treeNode *root, string name) {
    if (root != NULL) {
        treeNode *temp;
        // Create a queue for BFS
        list<treeNode *> queue;
        queue.push_back(root);
        while(!queue.empty()) {
            temp = queue.front();
            if (temp->data->name == name) {
                // found it
                break;
            }
            queue.pop_front();
            for (int i = 0; i < (temp->children).size(); i++) {
                queue.push_back(temp -> children[i]);
            }
        }
        return temp;
    }
    return NULL;
}

void addChild(treeNode* parent, treeNode* child) {
    //cout << "hello here" << endl;
    parent->children.push_back(child);
    child->parent = parent;
}

// Get timestamp
string getTimeStamp() {
    chrono::system_clock::time_point today  = chrono::system_clock::now();
    std::time_t tt = chrono::system_clock::to_time_t (today);
    string time_stamp = ctime(&tt);
    return time_stamp.substr(4,12);
 }

// Used to merge blocks in the Ldisk
void mergeLDisk() {
    list<diskBlock*>::iterator current, next;
    current = diskBlocks.begin();

    while (current != diskBlocks.end()) {
        next = current;
        advance (next,1);
        // case 1: only one disk block
        if (next == diskBlocks.end()) {
            break;
        }
        // case 2: the two blocks need to be merged
        if ((*current)->isFree == (*next)->isFree) {
            (*current)->end = (*next)->end;
            // erase from list
            delete *next;
            diskBlocks.erase(next);
        }
        else {
            advance (current,1);
        }
    }
}

void splitLDisk(vector<int> deadBlocks) {
    // make iterator for
}

void printFileInfo(fileOrDir *file) {
    cout << "File Name: " << file->name << " File Size: " << file->fileSize << " Timestamp: " << file->timeStamp << endl;
    list<int>::iterator it;
    cout << "Block addresses: ";
    for (it = file->blockAddresses.begin(); it != file->blockAddresses.end(); it++) {
        cout << *it << " ";
    }
    cout << endl;
}

void allocateBlocks(fileOrDir *file, int blockSize) {
    //cout << file->fileSize << "fileSize" << endl;
    int diff = file->fileSize - file->allocatedBytes;
    bool success = false;

    int numBlocksNeeded = ceil((diff * 1.0) / blockSize);
    if (numBlocksNeeded == 0) {
        return;
    }

    //cout << numBlocksNeeded << "numBlocksNeeded" << endl;
    list<diskBlock*>::iterator blockIterator;
    blockIterator = diskBlocks.begin();
    while (numBlocksNeeded > 0 && blockIterator != diskBlocks.end()) {
        // found a free disk block
        if((*blockIterator)->isFree == true) {
            int numFreeBlocks = (*blockIterator)->end - (*blockIterator)->start + 1;
            //cout << numBlocksNeeded << "numBlocksNeeded" << endl;
            //cout << numFreeBlocks << "numFreeBlocks" << endl;
            // case 1: numFreeBlocks < numBlocksNeeded
            if (numFreeBlocks < numBlocksNeeded) {
                // the block is used now
                (*blockIterator)->isFree = false;
                file->allocatedBytes += numFreeBlocks * blockSize;
                numBlocksNeeded -= numFreeBlocks;
                // add block addresses to the file
                for (int i = (*blockIterator)->start; i<= (*blockIterator)->end; i++) {
                    file->blockAddresses.push_back(i*blockSize);
                }
                // back to check condition
            }
            // case 2: numFreeBlocks >= numBlocksNeeded
            else  {
                // add block addresses to the file
                // cout << "here" << endl;
                for (int i = (*blockIterator)->start; i < (*blockIterator)->start + numBlocksNeeded; i++) {
                    file->blockAddresses.push_back(i*blockSize);
                }
                // allocate a new disk block
                diskBlock *tempBlock = new diskBlock;
                tempBlock->start = (*blockIterator)->start;
                tempBlock->end = (*blockIterator)->start + numBlocksNeeded - 1;
                tempBlock->isFree = false;
                // inserting new disk block before the disk block at the specified position
                diskBlocks.insert(blockIterator,tempBlock);
                // split the block
                (*blockIterator)->start = tempBlock->end + 1;
                file->allocatedBytes += numFreeBlocks * blockSize;
                numBlocksNeeded = 0;
                success = true;
                break;
            }
        }
        // end of if statement
        else {
            blockIterator++;
        }
    }

    // unable to find free blocks
    if (success == false) {
        // disk is full
        cout << "Error: disk is full!" << endl;
        file->fileSize = file->allocatedBytes;
        mergeLDisk();
        return;
    }

    mergeLDisk();
}

int main(int argc, char** argv) {
    //terminal path vector for interactive env
    vector<string> TERMINAL_PATH_VECTOR;
    TERMINAL_PATH_VECTOR.push_back("./");
    string TERMINAL_PATH = "./";

    vector<string> DIR_LIST_VECTOR;
    int diskSize;
    int blockSize;
    char *fl; //file_list
    char *dl; //dir_list
    // Format: -f <file_list.txt> -d <dir_list.txt> -s <disk size> -b <block size>
    // Check arguments
    if (argc != 9) {
        cout << "Error: Expected 8 arguments" << endl;
        return -1;
    }
    for (int i = 1; i < argc - 1; i = i + 2) {

        if  (!strcmp(argv[i], "-s")) {
            if (argv[i+1]) {
                diskSize = atoi(argv[i+1]);
            }
            else {
                cout << "ERROR: Invalid value for disk size" << endl;
            }
        }

        if (!strcmp(argv[i], "-b")) {
            if (argv[i+1]) {
                blockSize = atoi(argv[i+1]);
            }
            else {
                cout << "ERROR: Invalid value for block size" << endl;
            }
        }

        else if (!strcmp(argv[i], "-f")) {
            // -f <file_list.txt>
            if (argv[i+1]) {
                fl = argv[i+1];
            }
        }

        else if (!strcmp(argv[i], "-d")) {
            // -d <dir_list.txt>
            if (argv[i+1]) {
                dl = argv[i+1];
            }
        }
    }
    // test
    //cout << dl << " " << fl << " " << diskSize << " " << blockSize << " " << "Ayy" << endl;
    // Constructor called
    diskBlock *dBlock = new diskBlock;

    dBlock->start = 0;
    dBlock->end = ceil(diskSize * 1.0 / blockSize) - 1;
    dBlock->isFree = true;
    diskBlocks.push_front(dBlock);

    // Initialize directories
    ifstream directories (dl);
    ifstream files (fl);
    root = new treeNode;
    currentDir = root;
    string line;
    int count = 0;

    if (directories) {
        while (getline(directories, line)) {
            if (line.size() != 0) {
                if (count == 0) {
                    fileOrDir *dir = new fileOrDir;
                    dir->name = line;
                    dir->fileSize = 0;
                    dir->isDirectory = true;
                    dir->timeStamp = "";
                    dir->allocatedBytes = 0;
                    root->data = dir;
                    root->parent = NULL;
                    DIR_LIST_VECTOR.push_back(line);
                    // test
                    // cout << dir->name << " " << dir->fileSize << " " <<dir->isDirectory <<
                    // " " << dir->timeStamp <<endl;
                }
                else {
                    fileOrDir *dir = new fileOrDir;
                    dir->name = line;
                    dir->fileSize = 0;
                    dir->isDirectory = true;
                    dir->timeStamp = "";
                    dir->allocatedBytes = 0;
                    //cout << line.substr(0, line.find_last_of("/")) << "substring" <<endl;
                    treeNode *parent = findNode(root, line.substr(0, line.find_last_of("/")));
                    // printDirChildren(root);
                    treeNode *child = new treeNode;
                    child->data = dir;
                    child->parent = NULL;
                    addChild(parent, child);
                    DIR_LIST_VECTOR.push_back(line);
                    // printDirChildren(parent);
                }
                count ++;
            }
            continue;
        }
        // cout << "hello5" << endl;
        directories.close();
    }else {
        cout << "ERROR: Unable to open dir_list.txt" << endl;
        return -1;
    }
    // printDirectory(root);

    string ID;
    int fileSize;
    string d1;
    string d2;
    string d3;
    string dir;
    string garbage;
    if (files) {
        while (files >> ID >> garbage >> garbage >> garbage >> garbage >> garbage >> fileSize >> d1 >> d2 >> d3 >> dir) {
            fileOrDir *file = new fileOrDir;
            file->name = dir;
            file->fileSize = fileSize;
            file->isDirectory = false;
            file->timeStamp =  d1 + " " + d2 + " " + d3;
            file->allocatedBytes = 0;
            cout << file->timeStamp << endl;

            treeNode* parent = findNode(root, dir.substr(0, dir.find_last_of("/")));
            treeNode* child = new treeNode;
            child->data = file;
            addChild(parent, child);
            // allocate blocks for the file
            allocateBlocks(file,blockSize);
            printFileInfo(file);
        }
        files.close();
    }
    else {
        cout << "ERROR: Unable to open file_list.txt" << endl;
        return -1;
    }
    cout << "print directory" << endl;
    printDirectory(root);



    char cwd[1024];
    for(;;){
        string input = "";
        string input2 = "";
        string input3 = "";

        //getcwd(cwd, sizeof(cwd));
        cout << "Enter a command to manipulate the file system: " << TERMINAL_PATH << "$";
        getline(cin, input);

        size_t first_space = input.find(" ");
        if(first_space!=string::npos){
            //do all the multiline commands here
            input2 = input.substr(first_space+1);
            input = input.substr(0, first_space);
            size_t second_space = input2.find(" ");
            if(second_space==string::npos){
                //do the two space stuff here
                if(input.compare("cd") == 0){
                    string cd_temp = TERMINAL_PATH + input2;
                    // if(chdir(input2.c_str())==-1){
                    //     cout << "Not a valid directory!" << endl;
                    // }
                    if (find(DIR_LIST_VECTOR.begin(), DIR_LIST_VECTOR.end(), cd_temp) != DIR_LIST_VECTOR.end()){
                      // Element in vector.
                      TERMINAL_PATH = cd_temp;
                      TERMINAL_PATH_VECTOR.push_back(cd_temp);
                    }else{
                        cout << cd_temp << " Is not a valid directory!" << endl;
                        cout << "Below are you list of valid directories :: " << endl;
                        for(int x = 0 ; x<DIR_LIST_VECTOR.size(); x++){
                            cout << DIR_LIST_VECTOR[x] << endl;
                        }
                        cout << "USAGE :: cd <VALID DIRECTORY>" << endl;
                    }
                }else if(input.compare("mkdir") == 0){
                    //TODO put in tree
                    if(input2.substr(0,1).compare("/") == 0){
                        cout << "mkdir: " << input2 << " Permission denied" << endl;
                    }else{
                        treeNode* the_parent = findNode(root, TERMINAL_PATH);
                        fileOrDir *dir = new fileOrDir;
                        dir->name = TERMINAL_PATH + "/" +input2;
                        dir->fileSize = 0;
                        dir->isDirectory = true;
                        dir->timeStamp = "";
                        dir->allocatedBytes = 0;

                        treeNode *child = new treeNode;
                        child->data = dir;
                        child->parent = NULL;
                        addChild(the_parent, child);
                        DIR_LIST_VECTOR.push_back(TERMINAL_PATH + "/" + input2);
                    }


                }else if(input.compare("create") == 0){
                    fileOrDir *newfile = new fileOrDir;
                    newfile->name = currentDir->data->name + "/" + input2;
                    newfile->fileSize = 0;
                    newfile->isDirectory = false;
                    newfile->timeStamp =  d1 + " " + d2 + " " + d3;
                    newfile->allocatedBytes = 0;
                    treeNode *child = new treeNode;
                    child->data = newfile;
                    child->parent = NULL;
                    addChild(currentDir, child);
                }else if(input.compare("delete") == 0){
                    //delete the file or directory
                    treeNode* found = findNode(root, input2);
                    if (found == NULL) {
                        cout << "Error: not find the file/directory" << endl;
                        return -1;
                    }
                    treeNode *parent = found->parent;
                    // if (parent->data->isDirectory) {
                    //   //todo
                    // }
                    // else {
                    //   //todo
                    // }
                }else{
                    cout << "Command not found!" << endl;
                }
            }else{
                //do the three space stuff here
                input3 = input2.substr(second_space+1);
                input2 = input2.substr(first_space+1,second_space);
                if(input.compare("append") == 0){
                    treeNode* found = findNode(root, input2);
                    if (found == NULL) {
                        cout << "Error: not find the file/directory" << endl;
                        return -1;
                    }
                    if (found->data->isDirectory) {
                        cout << "Error: please give a file name" << endl;
                    }
                    else {
                        found->data->fileSize += stoi(input3);
                        allocateBlocks(found->data,blockSize);
                    }
                }else if(input.compare("remove") == 0){
                    //TODO
                }else{
                    cout << second_space << endl;
                    cout << "Command not AHHH found!" << endl;
                }
            }
        }else{
            //all of the one thing commands
            if(input.compare("cd..") == 0){
                if(TERMINAL_PATH_VECTOR.size()!=1){
                    TERMINAL_PATH_VECTOR.pop_back();
                    TERMINAL_PATH = rebuildTerminalPath(TERMINAL_PATH_VECTOR);
                }
            }else if(input.compare("ls") == 0){
                treeNode* temp_currentNode = findNode(root, TERMINAL_PATH);
                for (int i = 0; i < (temp_currentNode->children).size(); i++) {
                    cout << temp_currentNode->children[i]->data->name << " is a " <<
                    ((temp_currentNode -> children[i] -> data -> isDirectory) ? "directory" : "file") << endl;
                }
                // cout << "ls thing!" << endl;
            }else if(input.compare("exit") == 0){
                return 0; //TODO:: Destroy everything
            }else if(input.compare("dir") == 0 ){
                printDirectory(root);
                // cout << "dir thing!" << endl;
            }else if(input.compare("prfiles") == 0){
                //TODO
                // print out all file information
                cout << "prfiles thing!" << endl;
            }else if(input.compare("prdisk") == 0){
                //TODO
                cout << "prdisk thing!" << endl;
            }else{
                cout << "Command not found!" << endl;
            }
        }
    }
    return 0;
}
