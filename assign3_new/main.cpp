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

#include <sys/wait.h>
#include <sys/types.h>
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

class Lfile {
public:
    unsigned long address;
    Lfile *next;
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
/*
 char* getTimeStamp() {
 chrono::system_clock::time_point today  = chrono::system_clock::now();
 std::time_t tt = chrono::system_clock::to_time_t (today);
 char *time_stamp = ctime(&tt);
 return time_stamp;
 }
 */


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


void printFileInfo(fileOrDir *file) {
    cout << "File Name: " << file->name << "File Size: " << file->fileSize << "Timestamp: " << file->timeStamp << endl;
    list<int>::iterator it;
    cout << "Block addresses: ";
    for (it = file->blockAddresses.begin(); it != file->blockAddresses.end(); it++) {
        cout << *it << " ";
    }
}


void allocateBlocks(fileOrDir *file, int blockSize) {
    cout << file->fileSize << "fileSize" << endl;
    int diff = file->fileSize - file->allocatedBytes;
    bool success = false;
    
    int numBlocksNeeded = ceil((diff * 1.0) / blockSize);
    if (numBlocksNeeded == 0) {
        return;
    }
    
    cout << numBlocksNeeded << "numBlocksNeeded" << endl;
    list<diskBlock*>::iterator blockIterator;
    blockIterator = diskBlocks.begin();
    while (numBlocksNeeded > 0 && blockIterator != diskBlocks.end()) {
        // found a free disk block
        if((*blockIterator)->isFree == true) {
            int numFreeBlocks = (*blockIterator)->end - (*blockIterator)->start + 1;
            cout << numBlocksNeeded << "numBlocksNeeded" << endl;
            cout << numFreeBlocks << "numFreeBlocks" << endl;
            
            
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
            
            treeNode* parent = findNode(root, dir.substr(0, dir.find_last_of("/")));
            treeNode* child = new treeNode;
            child->data = file;
            addChild(parent, child);
            // allocate blocks for the file
            allocateBlocks(file,blockSize);
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
        cout << cwd << "/Enter a command to manipulate the file system:: ";
        getline(cin, input);
        
        size_t first_space = input.find(" ");
        if(first_space!=string::npos){
            //do all the multiline commands here
            input2 = input.substr(first_space+1);
            input = input.substr(0, first_space);
            size_t second_space = input2.find(" ");
            if(second_space==string::npos){
                //do the two space stuff here
                cout << input << endl;
                if(input.compare("cd") == 0){
                    
                }else if(input.compare("mkdir") == 0){
                    
                }else if(input.compare("create") == 0){
                    
                }else if(input.compare("delete") == 0){
                    
                }else{
                    cout << "Command fuck found!" << endl;
                }
            }else{
                //do the three space stuff here
                input3 = input2.substr(second_space+1);
                input2 = input2.substr(first_space+1,second_space);
                if(input.compare("append") == 0){
                    
                }else if(input.compare("remove") == 0){
                    
                }else{
                    cout << second_space << endl;
                    cout << "Command not AHHH found!" << endl;
                }
            }
        }else{
            //all of the one thing commands
            if(input.compare("cd..") == 0){
                cout << "Cd thing!" << endl;
            }else if(input.compare("ls") == 0){
                for (int i = 0; i < (currentDir->children).size(); i++) {
                    cout << currentDir->children[i] -> data -> name << " is a " <<
                    ((currentDir -> children[i] -> data -> isDirectory) ? "directory" : "file") << endl;
                }
                // cout << "ls thing!" << endl;
            }else if(input.compare("exit") == 0){
                return 0; //TODO:: Destroy everything
            }else if(input.compare("dir") == 0 ){
                printDirectory(root);
                // cout << "dir thing!" << endl;
            }else if(input.compare("prfiles") == 0){
                // print out all file information
                cout << "prfiles thing!" << endl;
            }else if(input.compare("prdisk") == 0){
                cout << "prdisk thing!" << endl;
            }else{
                cout << "Command not found!" << endl;
            }
        }
    }
    
    return 0;
}
