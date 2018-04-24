
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
    diskBlock *next;
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


int calculateBytesUnused(int fileSize, int blockSize){
	if(fileSize ==0)
		return 0;
	while (fileSize > blockSize)
		fileSize -= blockSize;
	return blockSize - fileSize;
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
      cout << "Child queue size: " << temp->children.size() << endl;
			for (int i = 0; i < temp->children.size(); ++i) {
        cout << "Child: " << temp->children[i]->data->name << endl;
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
      cout << "Current Dir: " << temp->data->name << " pen15: " << name << endl;
      if (temp->data->name == name) {
        cout << "Found Dir: " << temp->data->name << endl;
        // found it
        break;
      }
      queue.pop_front();
      for (int i = 0; i < (temp->children).size(); i++) {
        cout << "Chilean miner: " << temp->children[i]->data->name << endl;
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


// void allocateBlocks(fileOrDir *f, int blockSize) {
// 	int diff = f->fileSize - f->allocatedBytes;
// 	int numBlocksNeeded = ceil((diff * 1.0) / blockSize);
// 	while (numBlocksNeeded > 0) {
// 	}
// }



// todo
void cd(char *directory);

void ls() {
	for (int i = 0; i < (currentDir->children).size(); i++) {
		cout << currentDir->children[i] -> data -> name << "is directory: " << currentDir -> children[i] -> data -> isDirectory << '\n';
	}
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
		cout << dl << " " << fl << " " << diskSize << " " << blockSize << " " << "Ayy" << endl;
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
					// test
					cout << "line: " << line << endl;
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
						cout << line.substr(0, line.find_last_of("/")) << "substring" <<endl;
          	treeNode *parent = findNode(root, line.substr(0, line.find_last_of("/")));

						printDirChildren(root);
						treeNode *child = new treeNode;
						child->data = dir;
						child->parent = NULL;
						addChild(parent, child);
						printDirChildren(parent);


						cout << count << endl;
        	}
        	count ++;
      	}
				continue;
		}
  		// cout << "hello5" << endl;
  		directories.close();
    }
  else {
    cout << "ERROR: Unable to open dir_list.txt" << endl;
    return -1;
  }
	// printDirectory(root);

  cout << "WE BE ADDIN FILS NAU" << endl;
	string ID;
	int fileSize;
	string d1;
	string d2;
	string d3;
	string dir;
	string garbage;
  //if (files) {
      while (files >> ID >> garbage >> garbage >> garbage >> garbage >> garbage >> fileSize >> d1 >> d2 >> d3 >> dir) {
        //if( files.eof() ) break;
        //getline(files, line);
        
        cout << "TESTER ::: " << dir << endl;
        //if( files.eof() ) break;
        // if (!()) {
        //     cout << "Done: Please check file_list.txt" << endl;
        //     break;
        //     //return -1;
        // }else{

        fileOrDir *file = new fileOrDir;
				file->name = dir;
				file->fileSize = fileSize;
				file->isDirectory = false;
				file->timeStamp =  d1 + " " + d2 + " " + d3;

        treeNode* parent = findNode(root, dir.substr(0, dir.find_last_of("/")));
        treeNode* child = new treeNode;
				child->data = file;
        addChild(parent, child);
        //}
    }
    files.close();
  // }else {
  //   cout << "ERROR: Unable to open file_list.txt" << endl;
  //   return -1;
  // }
  cout << "ABOUT OT PRINT MOTHA FUCKER!!!!" <<endl;
  printDirectory(root);



  return 0;
}
