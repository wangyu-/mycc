#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <stack>
using namespace std;
const int _out=1;

enum asm_format_t{format_bin=0,format_elf32,format_win32};
const char format_name_table[][100]={"bin","elf32","win32"};

typedef char* cptr;

struct token_t  //struct of token
{
	string s;
	int n;
	int p;
	int line;
};
struct grammar_t;
typedef grammar_t* gptr;
struct grammar_t  //struct of grammar tree
{
	string n;
	int v;
	int p;//格式,横排 竖排
	//quater *qp;
	vector <gptr> l;
	void c()
	{
		n="";
		v=0;
		p=0;
		l.clear();
	}
	grammar_t()
	{
		n="";
		v=0;
		p=0;
		l.clear();
	}
	int prt(int level=0)
	{
		int i,j;
		for(j=0;j<level;j++) printf("  ");
		printf("%s\n",n.c_str());
		/*if(g->n=="quat")
		  {
		  g->qp->prt(n+1);
		  }
		  else*/
		for(i=0;i<l.size();i++)
		{
			l[i]->prt(level+1);
		}
		return 0;
	}
};

struct quat_t //quaternion
{
	string s1,s2,s3,s4;
};

struct var_type_t  //type for variable table
{
	string s;
	int addr;
	string func;
	vector <string> para;
	int n;
	void clear()
	{
		n=0;
		s="";
		addr=-1;
		func="";
		para.clear();
	}
};
