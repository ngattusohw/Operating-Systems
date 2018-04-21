#include "linkedlist.h"
#include "Gtree.h"
#include "parser.h"
#include "main.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <list>
#include <cmath>


using namespace std;
using std::string;

class File
{
    
    string name;
    int size;
    time_t timestamp;
    bool isDirectory;
    long fileSize;
    list<int> blockAddresses;
};

class Lfile {
    unsigned long address;
    int bytesUsed;
    struct Lfile *next;
};

class diskBlock {
    int startID;
    int endID;
    bool isFree;
};


class Tree
{
    File *file;
    vector<Tree *> children;
    Tree *parent;
};


int main(int argc, char** argv) {
    int diskSize;
    int blockSize;
    FILE *fl; //file_list
    FILE *dl; //dir_list
    parser the_parser; //the parser
    
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
                the_parser.fileList(argv[i+1]);
                // fl = fopen(argv[i+1], "r");//comment out
                // //do file list parsing
                // if (fl == NULL) {
                //     cout << "ERROR: Unable to open file_list.txt" << endl;
                //     return -1;
                // }
            }
        }
        
        
        else if (!strcmp(argv[i], "-d")) {
            // -d <dir_list.txt>
            if (argv[i+1]) {
                the_parser.dirList(argv[i+1]);

                // dl = fopen(argv[i+1], "r");

                // //do dir list parsing
                // if (dl == NULL) {
                //     cout << "ERROR: Unable to open dir_list.txt" << endl;
                //     return -1;
                // }
            }
            
        }
    }
}





