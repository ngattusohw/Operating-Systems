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

	while (inFile)
	{
		inFile.getline(oneline, MAXLINE);
		cout << oneline << endl; //something with Dir
	}

	inFile.close();
}
