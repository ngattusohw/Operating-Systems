// Nicholas Gattuso 
// Kexian Wu
// Louis Rozencwajg-Hays
// I pledge my honor that I have abidied by the stevens honor system

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
#include <typeinfo>

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
    int start; //start block ID
    int end; //end block ID
    bool isFree;
};


class treeNode {
public:
    fileOrDir *data; // either a file or a directory
    vector<treeNode*> children;
    treeNode *parent;
};

// all global variables
list<diskBlock*> diskBlocks;
treeNode *root = new treeNode;
treeNode *currentDir = new treeNode;

//A function that rebuilds the terminal path preview
string rebuildTerminalPath(vector<string> vec){
    string returnVal = "";
    for(int x = 0; x<vec.size(); x++){
        returnVal += vec[x];
    }
    return returnVal;
}

// bytes unused = allocated bytes - used bytes
int calculateBytesUnused(int fileSize, int blockSize) {
    if (fileSize == 0) {
        return 0;
    }
    int blocks = ceil(((float)fileSize / (float)blockSize));
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

// Print all information about the file (file Name, file size, timestamp)
void printFileInfo(fileOrDir *file) {
    cout << "File Name: " << file->name << " File Size: " << file->fileSize << " Timestamp: " << file->timeStamp << endl;
    list<int>::iterator it;
    cout << "Block addresses: ";
    for (it = file->blockAddresses.begin(); it != file->blockAddresses.end(); it++) {
        cout << *it << " ";
    }
    cout << endl;
}

// Print all file information
void printAllFiles(treeNode *root) {
    if (root != NULL) {
        treeNode *temp = root;
        list<treeNode *> queue;
        queue.push_back(root);
        while(!queue.empty()){
            // print the current directory name
            temp = queue.front();
            if(temp!=NULL){
                if(!(temp->data->isDirectory)){
                    printFileInfo(temp->data);
                }
            }
            //cout << temp->data->name << endl;
            queue.pop_front();
            for (int i = 0; i < temp->children.size(); ++i) {
                queue.push_back(temp -> children[i]);
            }
        }
    }
}

// Count the number of unused bytes for whole file system
int countFragmentation(treeNode *root, int blockSize) {
    int frag = 0;
    if (root != NULL) {
        treeNode *temp = root;
        list<treeNode *> queue;
        queue.push_back(root);
        while(!queue.empty()){
            // print the current directory name
            temp = queue.front();
            if(temp!=NULL){
                if(!(temp->data->isDirectory)){
                    //cout << "Calculating the frag! " << temp->data->fileSize << " " << blockSize <<endl;
                    frag += calculateBytesUnused(temp->data->fileSize, blockSize);
                }
            }
            queue.pop_front();
            for (int i = 0; i < temp->children.size(); ++i) {
                queue.push_back(temp -> children[i]);
            }
        }
    }

    return frag; //return the fragmentation
}



// Print out sub-directories under a given node
void printDirChildren(treeNode *node) {
    for (int i = 0; i < (node->children).size(); ++i)
    {
        if(node->children[i]->data->isDirectory == true){
            cout << node->children[i]->data->name << endl;
        }
    }
    cout << endl;
}

// To find one specific node in the tree through BFS
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
                return temp;
            }
            queue.pop_front();
            for (int i = 0; i < (temp->children).size(); i++) {
                queue.push_back(temp -> children[i]);
            }
        }
        return NULL;
    }
    return NULL;
}

// Adds child node to its parent node (append to the end of the list)
void addChild(treeNode* parent, treeNode* child) {
    parent->children.push_back(child);
    child->parent = parent;
}

// Deletes the child from its parent
int deleteChild(treeNode* parent, treeNode* child) {
    int test = 0;
    // to find the child
    for (int i = 0;i<parent->children.size();i++) {
        if (parent->children[i]->data->name == child->data->name) {
            // delete *next;
            parent->children.erase(parent->children.begin()+i);
            test = 1;
        }
    }
    child->parent = NULL;
    delete child->data;
    child->data = NULL;
    delete child;
    // test = 1 means successfully delete the child from its parent and test = 0 means the opposite
    return test;
}


// Get timestamp (current tie) ex: Apr 13 20:12
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
        // move to next node
        else {
            advance (current,1);
        }
    }
}

// Allocate file blocks to one specific file
void allocateBlocks(fileOrDir *file, int blockSize) {
    //cout << file->fileSize << ":: fileSize ... allocatedBytes ::" << file->allocatedBytes << endl;
    int diff = (file->fileSize) - (file->allocatedBytes);
    bool success = false;

    //cout << "This is testing mother fucker (diff, blocksize) " << diff << ", " << blockSize << endl;
    int numBlocksNeeded = ceil((float)diff / (float)blockSize);
    if (numBlocksNeeded == 0) {
        return;
    }

    list<diskBlock*>::iterator blockIterator;
    blockIterator = diskBlocks.begin();
    while (numBlocksNeeded > 0 && blockIterator != diskBlocks.end()) {
        //cout << "In the while loop, end ";
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
                // edge case: if node after is free and contains one block
                if ((*blockIterator)->start == (*blockIterator)->end &&
                    (*blockIterator)->isFree == true) {
                    list<diskBlock*>::iterator tempIterator = blockIterator;
                    advance(blockIterator,1);
                    tempIterator = diskBlocks.erase(tempIterator);
                }
                (*blockIterator)->start = tempBlock->end + 1;
                file->allocatedBytes += numBlocksNeeded * blockSize;
                numBlocksNeeded = 0;
                success = true;
                //cout << "Testing append block " << endl;
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
        cout << "Error: fail to allocate file blocks" << endl;
        file->fileSize = file->allocatedBytes;
        mergeLDisk();
        return;
    }

    mergeLDisk();
}




// Deallocate right number of file blocks from one specific file
void deallocateBlocks(fileOrDir *file, int blockSize) {
    // delete a number of bytes from the file
    int diff = file->allocatedBytes - file->fileSize;
    int numBlocksRemove = diff/blockSize;
    int totalBlocksRemove = numBlocksRemove;
    bool success = false;

    while (numBlocksRemove>0) {
        //cout << "Amount of blocks :: " << file->blockAddresses.size() << endl;
        list<int>::iterator addressIterator;
        addressIterator = file->blockAddresses.end();
        // the last node of the linked List
        addressIterator--;
        // calculate the blockID to remove
        //cout << "This is the addressIterator " << *addressIterator << endl;
        int blockID = *addressIterator / blockSize;
        //cout << "This is the blockId " << blockID << endl;
        list<diskBlock*>::iterator blockIterator;
        blockIterator = diskBlocks.begin();
        while (blockIterator != diskBlocks.end()) {
            if((*blockIterator)->start <= blockID && (*blockIterator)->end >= blockID){
                //split
                //cout << "BLOCK ID " << blockID << endl;
                diskBlock *newBlock = new diskBlock;
                newBlock->start = blockID;
                newBlock->end = blockID;
                newBlock->isFree = true;
                if(blockID == (*blockIterator)->end){
                    //only removing the last block
                    list<diskBlock*>::iterator nextNode = blockIterator;
                    advance(nextNode,1);

                    if(nextNode == diskBlocks.end()){
                        //cout << "Only removing the last block, where nextNode = diskBlocks end" << endl;
                        diskBlocks.push_back(newBlock);
                    }else{
                        //not adding at the back, need to figure out where to add
                        //cout << "Only removing the last block , and idk BLOCK ID :: " << blockID << endl;
                        (*blockIterator)->end = blockID-1;
                        advance(blockIterator,1);
                        diskBlocks.insert(blockIterator, newBlock);
                    }
                }else if(blockID == (*blockIterator)->start){
                    //cout << "Removing the front " << endl;
                    //removing the front
                    (*blockIterator)->start = blockID+1;
                    diskBlocks.insert(blockIterator, newBlock);
                }else{
                    //cout << "Removing somewhere in the middle " << endl;
                    //not the front, somewhere in the middle
                    diskBlock *whateverYouWantIDontCare = new diskBlock;
                    whateverYouWantIDontCare->start = blockID+1;
                    whateverYouWantIDontCare->end = (*blockIterator)->end;
                    whateverYouWantIDontCare->isFree = false;

                    (*blockIterator)->end = blockID-1;

                    list<diskBlock*>::iterator nextNode = blockIterator;
                    advance(nextNode,1);

                    if(nextNode == diskBlocks.end()){
                        //cout << "Removing somewhere in the middle where nextNode = diskBlocks end" << endl;
                        diskBlocks.push_back(newBlock);
                        diskBlocks.push_back(whateverYouWantIDontCare);
                    }else{
                        //not adding at the back, need to figure out where to add
                        //cout << "Removing somewhere in the middle where nextNode != diskBlocks end BLOCK ID :: " << blockID << endl;
                        advance(blockIterator,1);
                        diskBlocks.insert(blockIterator, whateverYouWantIDontCare);
                        diskBlocks.insert(prev(blockIterator), newBlock);
                    }
                }
            }
            advance(blockIterator,1);
            //cout << " Only once? " << endl;
        }
        numBlocksRemove--;
        // update the file info
        file->allocatedBytes -= blockSize;
        // remove the block
        // file->blockAddresses.erase();
        file->blockAddresses.pop_back();

    }

    list<diskBlock*>::iterator edgeCaseChecker;
    edgeCaseChecker = diskBlocks.begin();

    if((*edgeCaseChecker)->end < 0){
        diskBlocks.pop_front();
    }
    mergeLDisk();
}


// Destroy the whole tree
void destroyTree(treeNode* root) {
    // TODO
    if(root !=NULL){
        treeNode* temp = root;
        treeNode* t;
        for (int i = 0; i<temp->children.size();i++) {
          if (temp->children[i]->children.size()>0) {
            destroyTree(temp->children[i]);
          }
          t = temp->children[i];
          temp->data = NULL;
          delete temp->data;
          temp = NULL;
          delete temp; //freed memory
          temp = t;
        }
        root = NULL;
    }
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
    dBlock->end = ceil((float)diskSize / (float)blockSize) - 1;
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

    //initalize files
    if (files) {
        while (files >> ID >> garbage >> garbage >> garbage >> garbage >> garbage >> fileSize >> d1 >> d2 >> d3 >> dir) {
            fileOrDir *file = new fileOrDir;
            file->name = dir;
            file->fileSize = fileSize;
            file->isDirectory = false;
            file->timeStamp =  d1 + " " + d2 + " " + d3;
            file->allocatedBytes = 0;
            //cout << file->timeStamp << endl;

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
    // cout << "print directory" << endl;
    // printDirectory(root);

    //TERMINIAL CODE

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
                    if (find(DIR_LIST_VECTOR.begin(), DIR_LIST_VECTOR.end(), cd_temp) != DIR_LIST_VECTOR.end()){
                        // Element in vector.
                        TERMINAL_PATH = cd_temp;
                        TERMINAL_PATH_VECTOR.push_back(cd_temp);
                    }else{
                        cout << cd_temp << " Is not a valid directory!" << endl;
                        cout << "Below are your list of valid directories :: " << endl;
                        for(int x = 0 ; x<DIR_LIST_VECTOR.size(); x++){
                            cout << DIR_LIST_VECTOR[x] << endl;
                        }
                        cout << "USAGE :: cd <VALID DIRECTORY>" << endl;
                    }
                }else if(input.compare("mkdir") == 0){
                    if(input2.substr(0,1).compare("/") == 0){
                        cout << "mkdir: " << input2 << " Permission denied" << endl;
                    }else{
                        fileOrDir *dir = new fileOrDir;
                        dir->name = TERMINAL_PATH + "/" +input2;
                        dir->fileSize = 0;
                        dir->isDirectory = true;
                        dir->timeStamp = getTimeStamp();
                        dir->allocatedBytes = 0;
                        if(root != NULL){
                            treeNode* the_parent = findNode(root, TERMINAL_PATH);
                            cout << "This is the found parent " << the_parent->data->name << endl;
                            treeNode *child = new treeNode;
                            child->data = dir;
                            child->parent = NULL;
                            addChild(the_parent, child);
                            DIR_LIST_VECTOR.push_back(TERMINAL_PATH + "/" + input2);
                        }else{
                            root = new treeNode;
                            root->data = dir;
                            root->parent = NULL;

                        }
                    }
                }else if(input.compare("create") == 0){
                    fileOrDir *newfile = new fileOrDir;
                    newfile->name = TERMINAL_PATH + "/" + input2;
                    newfile->fileSize = 0;
                    newfile->isDirectory = false;
                    newfile->timeStamp =  getTimeStamp();
                    newfile->allocatedBytes = 0;

                    if(root != NULL){
                        treeNode* the_parent = findNode(root, TERMINAL_PATH);
                        treeNode *child = new treeNode;
                        child->data = newfile;
                        child->parent = NULL;
                        addChild(the_parent, child);
                    }else{
                        root = new treeNode;
                        root->data = newfile;
                        root->parent = NULL;
                    }
                }else if(input.compare("delete") == 0){
                    //delete the file or directory
                    if(input2.compare("./") == 0 ){
                        cout << "You cannot delete the root directory!" << endl;
                    }else{
                        treeNode* found = findNode(root, input2);
                        if (found == NULL) {
                            cout << "Error: not find the file/directory" << endl;
                        }else{
                            treeNode *parent = found->parent;
                            if (found->data->isDirectory) {
                                // the one found is a directory
                                if (found->children.size()!= 0) {
                                    cout << "Error: directory is not empty" << endl;
                                }
                                // delete the directory
                                else {
                                    deleteChild(parent, found);
                                }
                            }else { // to delete a file
                                // deallocate file blocks
                                found->data->fileSize = 0;
                                deallocateBlocks(found->data,blockSize);
                                // erase the file
                                deleteChild(parent, found);
                                // update the timestamp of the parent node
                                parent->data->timeStamp = getTimeStamp();
                            }
                        }
                    }
                }else{
                    cout << "Command not found!" << endl;
                }
            }else{
                //do the three space stuff here
                input3 = input2.substr(second_space+1);
                input2 = input2.substr(0,second_space);
                cout << "This is input 2 ::" << input2 << "::"<< endl;
                if(input.compare("append") == 0){
                    if(typeid(input2) == typeid(string)){
                        treeNode* found = findNode(root, input2);
                        if (found == NULL) {
                            cout << "Error: not find the file/directory" << endl;
                        }else if (found->data->isDirectory) {
                            cout << "Error: please give a file name" << endl;
                        }
                        else {
                            cout << "This is the found treeNode" << found->data->name << endl;
                            try{
                                int bytesToAdd = stoi(input3);
                                if (found->data->fileSize + stoi(input3) > diskSize) {
                                    cout << "Error: you cannot add " << bytesToAdd <<
                                    " bytes " << "to file" << found->data->name << endl;
                                }
                                else {
                                    found->data->fileSize += stoi(input3);
                                    found->data->timeStamp = getTimeStamp();
                                    //cout << " Printing out the input3 " << input3 << endl;
                                    allocateBlocks(found->data,blockSize);
                                }
                            }catch(...){
                                cout << "Please enter the correct parameter types! Usage:: append <filename:string> <bytes:int>" << endl;
                            }
                        }
                    }else{
                        cout << "Incorrect argument types. Usage: append <filename:string> <bytes:int>" << endl;
                    }
                }else if(input.compare("remove") == 0){
                    treeNode* found = findNode(root, input2);
                    if (found == NULL) {
                        cout << "Error: not find the file/directory" << endl;
                    }else if (found->data->isDirectory) {
                        cout << "Error: please give a file name" << endl;
                    }else{
                        cout << "This is the found treeNode" << found->data->name << endl;
                        try{
                            int bytesToRemove = stoi(input3);
                            //cout << " Printing out the input3 " << input3 << endl;
                            if (found->data->fileSize < bytesToRemove) {
                                cout << "Error: you cannot remove " << bytesToRemove <<
                                " bytes " << "from file" << found->data->name << endl;
                            }
                            else {
                                found->data->fileSize -= bytesToRemove;
                                found->data->timeStamp = getTimeStamp();
                                deallocateBlocks(found->data,blockSize);                            }
                        }catch(...){
                            cout << "Please enter the correct parameter types! Usage:: remove <filename:string> <bytes:int>" << endl;
                        }
                    }
                }else{
                    cout << "Command not found!" << endl;
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
                if(temp_currentNode != NULL){
                    for (int i = 0; i < (temp_currentNode->children).size(); i++) {
                        cout << temp_currentNode->children[i]->data->name << " is a " <<
                        ((temp_currentNode -> children[i] -> data -> isDirectory) ? "directory" : "file") << endl;
                    }
                }
                // cout << "ls thing!" << endl;
            }else if(input.compare("exit") == 0){
                // deallocate data structures and exit program
                string response;
                cout << "Are you sure you want to exit (y/n): " << endl;
                cin >> response;
                if (response.compare("y") == 0) {
                    // Destroy everything
                    destroyTree(root);
                    root = NULL;
                    cout << "Exiting...." << endl;
                    return 0;
                }
            }else if(input.compare("dir") == 0 ){
                printDirectory(root);
                // cout << "dir thing!" << endl;
            }else if(input.compare("prfiles") == 0){
                // print out all file information
                printAllFiles(root);
            }else if(input.compare("prdisk") == 0){
                for (list<diskBlock*>::iterator it = diskBlocks.begin(); it != diskBlocks.end(); ++it){
                    if((*it)->isFree){
                        cout << "Free: " << (*it)->start << " - " << (*it)->end << endl;
                    }else{
                        cout << "In use: " << (*it)->start << " - " << (*it)->end << endl;
                    }
                }

                cout << "Block size :: " << blockSize <<" , Fragmentation :: " << countFragmentation(root, blockSize) << endl;
            }else{
                cout << "Command not found!" << endl;
            }
        }
    }
    return 0;
}
