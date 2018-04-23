#ifndef _MAIN_H_
#define _MAIN_H_

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <list>
#include <cmath>
#include <cstdlib>
using namespace std;



class fileOrDir {
public:
    string name;
    time_t timestamp;
    bool isDirectory; // either a regular file or a directory
    list<int> blockAddresses; // for file
    long fileSize;
    fileOrDir(string n, int size, bool isdirectory) {
    name = n;
    fileSize = size;
    isDirectory = isdirectory;
  }
};

class Lfile {
public:
    unsigned long address;
    int bytesUsed;
    Lfile *next;
};

class diskBlock {
public:
    int start;
    int end;
    bool isFree;
    diskBlock *next;
    diskBlock(int s, int e, bool isfree) {
        start = s;
        end = e;
        isFree = isfree;
    }
};


class treeNode {
public:
    fileOrDir *fileOrDir;
    vector<treeNode *> children;
    treeNode *parent;
    treeNode(class fileOrDir *f) {
    fileOrDir = f;
  }
};

int calculateBytesUnused(int fileSize, int blockSize);
void addChild(treeNode* parent, treeNode* child);
treeNode* findNode(treeNode *root, string name);
treeNode* deleteChild(treeNode *parent, treeNode *child);

#endif
