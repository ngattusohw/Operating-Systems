#include "linkedlist.h"
#include "Gtree.h"
#include <stdio.h>
#include "main.h"
#include "parser.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <list>
#include <cmath>


using namespace std;
using std::string;
int MAXLINE = 256;

parser::parser(){

}

parser::~parser(){

}

void parser::dirList(string file){
	ifstream inFile (file);
	char oneline[MAXLINE];

	while (inFile)
	{
		inFile.getline(oneline, MAXLINE);
		cout << oneline << endl; //something with Dir
	}

	inFile.close();

}

void parser::fileList(string file){
	ifstream inFile (file);
	char oneline[MAXLINE];
	string ID;
	string fileSize;
	string date1;
	string date2;
	string date3;
	string filename;
	string garbage;
	while (inFile)
	{
		inFile.getline(oneline, MAXLINE);
		inFile >> ID >> garbage >> garbage >> garbage >> garbage >> garbage >> fileSize >> date1 >> date2 >> date3 >> filename;
		cout << ID << " fileSize:: " << fileSize << " Date:: " << date1 << date2 << date3 << " File name::" << filename << endl; //something with Dir
	}

	inFile.close();
}
