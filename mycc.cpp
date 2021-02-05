//#include <windows.h>
//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
char format_name_table[][100]={"bin","elf32","win32"};
asm_format_t g_asm_format=format_elf32;
string g_format_name;
//struct quater;
//string intext;
struct acc
{
struct tk
{
	string s;
	int n;
	int p;
	int line;
}ttmp;
int isnum(char c)   //判断是否是数字
{
	return c>='0'&&c<='9';
}
int islet(char c)  //判断是否是英文字母或下滑线
{
	return c=='_'||c>='a'&&c<='z'||c>='A'&&c<='Z';
}
int issy(char c)   //判断是符号
{
	return c=='.'||c=='+'||c=='-'||c=='*'||c=='/'||c=='='||c=='>'||c=='<'||c=='!'||c=='|'||c=='&'||c=='%'||c=='~'||c=='('||c==')'||c=='{'||c=='}'||c==';'||c==','||c=='['||c==']'||c==':';
}
int ifconstant(char *c) //如果c所指向的文本是常数,返回常数占的长度,否则返回
{
	int i,j,k;
	if(c[0]=='\"')
	{
		for(i=1;c[i]!=0;i++)
		{
			if(c[i]=='\"') {i++;break;}
		}
		return i;
	}
		if(c[0]=='\'')
	{
		for(i=1;c[i]!=0;i++)
		{
			if(c[i]=='\'') {i++;break;}
		}
		return i;
	}

	if(!isnum(c[0])) return 0;
	for(i=1;;i++)
	{
		if(!isnum(c[i])&&c[i]!='.') break;
	}
	if(islet(c[i]))
	{
	//printf("<error1>");
	}
	return i;
}
int ifsymbol(char *c)//如果c所指向的文本是符号,返回0
{
	int i,j,k;
	if(!issy(c[0])) return 0;
	return 1;
}
int ifword(char *c)//如果c所指向的文本是单词,返回单词占的长度,否则返回 (单词包括保留字和自定义字符)
{
	int i,j,k;
	if(!islet(c[0])) return 0;
	for(i=1;;i++)
	{
		if(!isnum(c[i])&&!islet(c[i])) break;
	}
	return i;
}
int ifasm(char *c)
{
	int i,j,k;
	if(memcmp(c,"__asm",5)!=0) return 0;
	return 5;
}
int note1(char *c)
{
	int i;
	if(memcmp(c,"//",2)==0)
	{
		for(i=2;c[i]!='\n'&&c[i]!=0;i++) ;
		return i;
	}
	return 0;
}
int note2(char *c,int &n)
{
	int i;
	n=0;
	if(memcmp(c,"/*",2)==0)
	{
		for(i=1;!(c[i+1]=='*'&&c[i+2]=='/');i++)
		{
			if(c[i]=='\n') n+=1;
			if(c[i]==0) return i;
		}
		return i+3;
	}
	return 0;
}
string cut(char *c,int n)//截取c所指向的n个字符
{
	//static char s[100*1000];  //asm token can be very large
	//memcpy(s,c,n);
	//s[n]=0;
	return string(c,c+n);
}
vector <tk> token;
typedef char* cptr;

int run(char *s,cptr &out,vector<tk> &tok)
{
	int i,j,k;
	static string output;
	char keyword[100][100]=
	{
	"if","else","for","break","continue","return","goto",
	"int","long","char","short","bool","float","double",
	"$end"
	};//关键字列表
	//char s[1000];
	map<string,int> mp;  //关键字映射表
	//vector<tk> token;   //token序列
   	for(i=0;strcmp(keyword[i],"$end")!=0;i++)
	{
		mp[keyword[i]]=i+3;
	}
	//gets(s);
	int l=strlen(s);
	int line=0;
	for(i=0;i<l;i+=0)//分析s
	{
		int r=0;
		if((r=ifasm(s+i))>0)
		{
			ttmp.p=i;
			ttmp.line=line;
			ttmp.n=-1;
			//i+=r;
			for(j=0;;j++)
			{
				if(s[i+j]=='{') break;
				if(i+j==l-1)
				{
					printf("__asm token incorrect1\n");
					out="";
					exit(-1);
				}
			}
			for(k=0;;k++)
			{
				if(s[i+k]=='}'&&(s[i+k+1]=='\n'||s[i+k+1]==0)) break;
				if(i+k==l-1)
				{
					printf("__asm token incorrect2\n");
					out="";
					exit(-1);
				}
			}
			ttmp.s=cut(s+i+j+1,k-j-1);
			token.push_back(ttmp);
			output+='#';
			i+=(k+1);
			//printf("<%s>",ttmp.s.c_str());
		}
		if((r=ifsymbol(s+i))>0)   //如果是分界符
		{
			int r2,n2;
			if((r2=note1(s+i))>0)
			{
				i+=r2;
				continue;
			}
			if((r2=note2(s+i,n2))>0)
			{
				i+=r2;
				line+=n2;
				continue;
			}
			ttmp.s=cut(s+i,r);
			ttmp.n=mp[ttmp.s];
			ttmp.p=i;
			ttmp.line=line;
			token.push_back(ttmp);
			output+=ttmp.s.c_str()[0];
			i+=r;
			continue;
		}
		if((r=ifconstant(s+i))>0)   //如果是常量
		{
			ttmp.s=cut(s+i,r);
			ttmp.n=2;
			ttmp.p=i;
			ttmp.line=line;
			token.push_back(ttmp);
			if(ttmp.s.c_str()[0]=='\"')
				output+='$'; //字符串常量
			else output+='@';//普通常量

			i+=r;
			continue;
		}
		if((r=ifword(s+i))>0)     //如果是单词
		{
			ttmp.s=cut(s+i,r);
			ttmp.n=mp[ttmp.s];
			ttmp.p=i;
			//printf("<%s>",ttmp.s.c_str());
			ttmp.line=line;
			if(ttmp.n==0)
			{
				ttmp.n=1;
				output+='w';
			}
			else
			{
				if(ttmp.s=="int"||ttmp.s=="long") output+='l';
				else if(ttmp.s=="char"||ttmp.s=="bool"||ttmp.s=="short") output+='l';
				else if(ttmp.s=="double"||ttmp.s=="float") output+='d';
				else output+=ttmp.s.c_str()[0];
			}
			token.push_back(ttmp);
			i+=r;
			continue;
		}
		if(s[i]==' '||s[i]=='	') {i++;continue;}
		if(s[i]=='\n') {i+=1;line+=1;/*output+='\n';*/continue;}
		printf("<unknow char: %02x>\n",(int)s[i]);
		exit(-1);
		output+='?';
		i++;
	}
	/*printf("%s\n",output.c_str());
	for(;;)
	{
	getchar();
	}*/
	tok=token;
	//out=new char[1000*1000];
	//strcpy(out,output.c_str());
	out=(char*)output.c_str();
	return 0;
}
};

struct syntax
{
/////////////////////////////////////////////////////syntaxbegin
vector <acc::tk> token;
char * _s0;
struct grammar;
typedef grammar* gptr;
struct grammar
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
	grammar()
	{
		n="";
		v=0;
		p=0;
		l.clear();
	}
};

typedef char* cptr;
int asm0;
map<string,string> asm_mp;
syntax()
{
	asm0=-1;
}
string gen_asm()
{
	asm0++;
	char tmp[100];
	sprintf(tmp,"%d",asm0);
	string r="?asm";
	return r+tmp;
}
///////////////////////////////
int rd(cptr &s,char c)
{
	if(*s==c) {s+=1;return 1;}
	return 0;
}
int rd(cptr &s,char *c,int &t)
{
	int l=strlen(c);
	if(memcmp(s,c,l)==0)  {s+=l;t=l;return 1;}
	return 0;
}
int rd(cptr &s,char *c,string &t)
{
	int l=strlen(c);
	t=c;
	if(memcmp(s,c,l)==0)  {s+=l;return 1;}
	return 0;
}
int rd(cptr &s,char *c)
{
	int l=strlen(c);
	if(memcmp(s,c,l)==0)  {s+=l;return 1;}
	return 0;
}
int okop/*表达式语句*/(cptr &s,gptr &g)
{
	g=new grammar;
	gptr gt;
	g->n="expression_statement";
	cptr t=s;
	if(okS(s,gt))
		if( rd(s,';') )
		{
			g->l.push_back(gt);
			return 1;
		}
	s=t;return 0;
}
int tb/*变量声明区*/(cptr &s,gptr &g)
{
	g=new grammar;
	g->n="var_declare_zone";
	gptr gt;
	while(okts(s,gt)) {g->l.push_back(gt);}
	return 1;
}
int fb/*函数区*/(cptr &s,gptr &g)
{
	g=new grammar;
	g->n="function_zone";
	gptr gt;
	while(okfs(s,gt)) {g->l.push_back(gt);}
	return 1;
}
int okif/*if语句*/(cptr &s,gptr &g)
{
	cptr t=s;
	g=new grammar;gptr gt;
	g->n="if_statement";
	if(rd(s,'i'))
		if(rd(s,'('))
			if(okS(s,gt))
			{
				gptr g2=new grammar;g2->n="expression";g->l.push_back(g2);g2->l.push_back(gt);
				//g->l.push_back(gt);
				//运算语句
				if(rd(s,')'))
					if(oks(s,gt))
					{
						g->l.push_back(gt);
						cptr t2=s;
						if(rd(s,'e'))
							if(oks(s,gt))
							{
								g->l.push_back(gt);
								return 1;
							}
						s=t2;return 1;
					}
			}
	s=t;return 0;
}
int okfor/*for语句*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt;
	g=new grammar;
	g->n="for";
	if(rd(s,"f("))
	{
		if(okS(s,gt)){gptr g2=new grammar;g2->n="expression";g->l.push_back(g2);g2->l.push_back(gt);}
		else {gptr g2=new grammar;g2->n="empty";g->l.push_back(g2);}
		if(rd(s,';'))
		{
		if(okS(s,gt)) {gptr g2=new grammar;g2->n="expression";g->l.push_back(g2);g2->l.push_back(gt);}
		else {gptr g2=new grammar;g2->n="empty";g->l.push_back(g2);}
			if(rd(s,';'))
			{
		if(okS(s,gt)) {gptr g2=new grammar;g2->n="expression";g->l.push_back(g2);g2->l.push_back(gt);}
		else {gptr g2=new grammar;g2->n="empty";g->l.push_back(g2);}
				if(rd(s,')'))
				{
					if(oks(s,gt)){g->l.push_back(gt); return 1;}
				}
			}
		}
	}
	s=t;return 0;
}
int okms(cptr &s,gptr &g)
{
	cptr t=s;
	g=new grammar;
	g->n="block_statement";
	gptr gt;
	if(rd(s,'{'))
	{
		while(oks(s,gt)) g->l.push_back(gt);
		if(rd(s,'}')) return 1;
	}
	s=t;return 0;
}
int okre(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt;
	g=new grammar;
	g->n="return";
	if(rd(s,'r'))
	{

		if(okS(s,gt))
		{
			gptr g2=new grammar;
			g2->n="expression";
			g2->l.push_back(gt);
			g->l.push_back(g2);
			if(rd(s,';'))
			return 1;
		}
	}
	s=t;
	return 0;
}
int okctn(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt;
	g=new grammar;
	g->n="continue";
	if(rd(s,'c'))
	{
			if(rd(s,';'))
			return 1;
	}
	s=t;
	return 0;
}
int okbrk(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt;
	g=new grammar;
	g->n="break";
	if(rd(s,'b'))
	{
			if(rd(s,';'))
			return 1;
	}
	s=t;
	return 0;
}
int oklb(cptr &s,gptr &g)
{
	cptr t=s;
	g=new grammar;
	g->n="label";
	gptr g1;
	g1=new grammar;
	// g1->n=token[s-_s0].s; //原位置 有bug
	//g->l.push_back(g1);
	if(rd(s,'w'))
		if(rd(s,':'))
		{
			g1->n=token[s-_s0-1-1].s;
			g->l.push_back(g1);
			return 1;
		}
	s=t;return 0;
}
int okgt(cptr &s,gptr &g)
{
	cptr t=s;
	g=new grammar;
	g->n="goto";
	gptr g1;
	g1=new grammar;

	if(rd(s,'g'))
	{
		g1->n=token[s-_s0].s;
		g->l.push_back(g1);
		if(rd(s,'w'))
		{
			if(rd(s,';'))
			return 1;
		}

	}
	s=t;return 0;
}
int okasm(cptr &s,gptr &g)
{
	cptr t=s;
	g=new grammar;
	g->n="asm_statement";
	gptr g2;
	if(rd(s,'#'))
	{
		string t1=gen_asm();
		g2=new grammar;
		g2->n=t1;
		g->l.push_back(g2);
		string t2=token[s-_s0-1].s;
		asm_mp.insert(pair<string,string>(t1,t2));
		return 1;
	}
	s=t;return 0;
}
int oks(cptr &s,gptr &g)
{
	cptr t=s;
	g=new grammar;
	g->n="undecided_statement";
	gptr gt;
	if(okasm(s,gt)){g=gt;return 1;}
	if(oklb(s,gt)) {g=gt;return 1;}
	if(okgt(s,gt)) {g=gt;return 1;}
	if(okif(s,gt)) {g=gt;return 1;}//if语句
	if(okfor(s,gt)){g=gt;return 1;}//for语句

	if(okop(s,gt)) {g=gt;return 1;}
	if(okms(s,gt)) {g=gt;return 1;}
	if(okre(s,gt)) {g=gt;return 1;}
	if(okctn(s,gt)){g=gt;return 1;}
	if(okbrk(s,gt)){g=gt;return 1;}
	if(rd(s,';')){g=new grammar;g->n="empty_statement";return 1;}
	s=t;
	return 0;
}
int sb(cptr &s,gptr &g)/*语句区*/
{
	
	cptr t=s;
	gptr gt;
	g=new grammar;
	g->n="statement_zone";
	while(oks(s,gt)) g->l.push_back(gt);

    return 1;
}
int okproc/*程序*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt;
	g=new grammar;
	g->n="program";
//line1:
//	int sign=0;
	if(tb(s,gt)) {g->l.push_back(gt);}
	if(fb(s,gt)) {g->l.push_back(gt);}
line1:
	int sign=0;
	if(tb(s,gt))
	{
		if(gt->l.size())sign=1;
		for(int i=0;i<gt->l.size();i++)
			g->l[0]->l.push_back(gt->l[i]);
	}
	if(fb(s,gt))
	{
		if(gt->l.size())sign=1;
		for(int i=0;i<gt->l.size();i++)
			g->l[1]->l.push_back(gt->l[i]);
	}
	if(sign) goto line1;

	//if(okft(s,gt)) g->l.push_back(gt);
	return 1;
}
int okfe/*变量声明*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt,gt1;
	if(okt(s,gt))
	{
		gt1=gt;
		if(okse(s,gt))
		{
			g=gt;g->l.push_back(gt1);
			while(rd(s,"[@]"))
			{
				gt1=new grammar;
				gt1->n=token[s-_s0-2].s;
				g->l.push_back(gt1);
			}
		    return 1;
		}
	}
	s=t;g=0;return 0;
}
int okfl/*函数参数表*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt;
	g=new grammar;
	g->n="parameter_list";
	if(okfe(s,gt))
	{
			g->l.push_back(gt);
			while(rd(s,','))
			{
				if(okfe(s,gt)){g->l.push_back(gt);continue;}
				s-=1;break;
			}
			return 1;
	}
	s=t;return 1;//一定能接受
}
int okfs/*函数*/(cptr &s,gptr &g)
{
	cptr t=s;
	g=new grammar;gptr gt;
	//g->n="function";
	if(okt(s,gt))
	{
		g->l.push_back(gt);
		if(rd(s,'w'))
		{
			g->n=token[s-_s0-1].s;
			if(rd(s,'('))
				if(okfl(s,gt))
				{
					g->l.push_back(gt);
					if(rd(s,')'))
					{
						if(rd(s,'{'))
						{
							if(tb(s,gt)) g->l.push_back(gt);
							if(sb(s,gt)) g->l.push_back(gt);
							if(rd(s,'}'))
									return 1;
						}
					}
				}
		}
	}
	s=t;return 0;
}
int okt/*数据类型*/(cptr &s,gptr &g)/*类型*/
{
	cptr t=s;
	gptr gt;
	g=new grammar;
	if(rd(s,'l')){while(rd(s,'*')); g->n="int";return 1;}//忽略掉所有*
	s=t;return 0;
}
int okse/*变量名*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt;
	g=new grammar;
	g->n=token[int(s-_s0)].s;
	if(rd(s,'w')) {return 1;}
	s=t;return 0;
}
int okts/*变量声明*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt,gt1;
	if(okt(s,gt))
	{
		gt1=gt;
		if(okse(s,gt))
		{
			g=gt;g->l.push_back(gt1);
			while(rd(s,"[@]"))
			{
				gt1=new grammar;
				gt1->n=token[s-_s0-2].s;
				g->l.push_back(gt1);
			}
			if(rd(s,';')){ return 1;}
		}
	}
	s=t;g=0;return 0;
}


int okft/*函数调用*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt;
	g=new grammar;
	if(rd(s,'w'))
	{
		g->n=token[s-_s0-1].s;
		if(rd(s,'('))
			if(okftl(s,gt))//参数表
			{
				g->l.push_back(gt);
				if(rd(s,')'))
					return 1;
			}
	}
	s=t;return 0;
}
int okftl/*函数调用参数表*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt;
	g=new grammar;
	g->n="parameter";
	if(okS(s,gt))
	{
		g->l.push_back(gt);
		while(rd(s,','))
		{
			if(okS(s,gt)) {g->l.push_back(gt);continue;}//函数调用是表达式
			s-=1;break;
		}
		return 1;
	}
	g->c();g->n="parameter";s=t;return 1;//肯定会接受
}


int okS/*&&||*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt,g1,g2;
	string o;
	if(okA(s,gt))
	{
		gptr g1=gt;
		while(rd(s,"||",o)||rd(s,"&&",o))
		{
			if(okA(s,gt))
			{
				g2=new grammar;
				g2->n=o;
				g2->l.push_back(g1);
				g2->l.push_back(gt);
				g1=g2;
				continue;
			}
			s-=o.length();break;
		}
		g=g1;
		return 1;
	}
	s=t;return 0;
}
int okA/*=*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt,g1,g2;
	string o;
	if(okE(s,gt))
	{
		gptr g1=gt;
		while(rd(s,"=",o))
		{
			if(okE(s,gt))
			{
				g2=new grammar;
				g2->n=o;
				g2->l.push_back(g1);
				g2->l.push_back(gt);
				g1=g2;
				continue;
			}
			s-=o.length();break;
		}
		g=g1;
		return 1;
	}
	s=t;return 0;
}
int okE/*== !=*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt,g1,g2;
	string o;
	if(okF(s,gt))
	{
		gptr g1=gt;
		while(rd(s,"==",o)||rd(s,"!=",o))
		{
			if(okF(s,gt))
			{
				g2=new grammar;
				g2->n=o;
				g2->l.push_back(g1);
				g2->l.push_back(gt);
				g1=g2;
				continue;
			}
			s-=o.length();break;
		}
		g=g1;
		return 1;
	}
	s=t;return 0;
}
int okF/*<><=>=*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt,g1,g2;
	string o;
	if(okH(s,gt))
	{
		gptr g1=gt;
		while(rd(s,"<=",o)||rd(s,">=",o)||rd(s,"<",o)||rd(s,">",o))
		{
			if(okH(s,gt))
			{
				g2=new grammar;
				g2->n=o;
				g2->l.push_back(g1);
				g2->l.push_back(gt);
				g1=g2;
				continue;
			}
			s-=o.length();break;
		}
		g=g1;
		return 1;
	}
	s=t;return 0;
}
int okH/*+-*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt,g1,g2;
	string o;
	if(okI(s,gt))
	{
		gptr g1=gt;
		while(rd(s,"+",o)||rd(s,"-",o))
		{
			if(okI(s,gt))
			{
				g2=new grammar;
				g2->n=o;
				g2->l.push_back(g1);
				g2->l.push_back(gt);
				g1=g2;
				continue;
			}
			s-=o.length();break;
		}
		g=g1;
		return 1;
	}
	s=t;return 0;
}
int okI/** /%*/(cptr &s,gptr &g)
{
	cptr t=s;
	gptr gt,g1,g2;
	string o;
	if(okJ(s,gt))
	{
		gptr g1=gt;
		while(rd(s,"*",o)||rd(s,"/",o)||rd(s,"%",o))
		{
			if(okJ(s,gt))
			{
				g2=new grammar;
				g2->n=o;
				g2->l.push_back(g1);
				g2->l.push_back(gt);
				g1=g2;
				continue;
			}
			s-=o.length();break;
		}
		g=g1;
		return 1;
	}
	s=t;return 0;
}
int okJ/*!*/(cptr &s,gptr &g)
{
	gptr gt,g1,g2;
	cptr t=s;
	string o;
	if(rd(s,"!",o)||rd(s,"&",o)||rd(s,"*",o))
		if(okL(s,gt))
		{
			g=new grammar;
			g->n=o;
			g->l.push_back(gt);
			return 1;
		}
	s=t;
	return okL(s,g);
}
int okL_notused/*[]*/(cptr &s,gptr &g)
{
	gptr gt,gt2,g1,g2;
	cptr t=s;
	string o;
	if(okZ(s,gt))
	{
		if(rd(s,'['))
			if(okS(s,gt2))
				if(rd(s,']'))
				{
					g=new grammar;
					g->n="[]";
					g->l.push_back(gt);
					g->l.push_back(gt2);
					return 1;
				}
	}
	s=t;
	return okZ(s,g);
}
int okL/*[]*/(cptr &s,gptr &g) //support multi array
{
	gptr gt,gt2,g1,g2;
	cptr t=s;
	string o;
	if(okZ(s,gt))
	{
		gptr g1=gt;
		while(rd(s,"[",o))
		{
			cptr t2=s;
			if(okS(s,gt)&&rd(s,']'))
			{
				g2=new grammar;
				g2->n="[]";
				g2->l.push_back(g1);
				g2->l.push_back(gt);
				g1=g2;
			}
			else
			{
				s=t2;//recover okS + "]"
				s-=o.length();//recover "["
				break;
			}
		}
		g=g1;
		return 1;
	}
	s=t;
	return 0;
}
int okZ(cptr &s,gptr &g)
{
	g=new grammar;
	cptr t=s;
	gptr gt;
	if(rd(s,'@')) {g->n=token[s-_s0-1].s;return 1;}
	if(rd(s,'$')) {g->n=token[s-_s0-1].s;return 1;}
	if(rd(s,'('))
		if(okS(s,gt))
			if(rd(s,')'))
			{
				g=gt;
				return 1;
			}
	s=t;
	if(okft(s,gt)) {g=gt;return 1;}
	if(rd(s,'w')) {g->n=token[s-_s0-1].s;return 1;}
	s=t;
	if(okZZ(s,gt)) {g=gt;return 1;}
	s=t;return 0;
}
int okZZ(cptr &s,gptr &g)
{
	cptr t=s;
	gptr g1;
	g=new grammar;
	g->n="array_constant";
	if(rd(s,'{'))
		if(rd(s,'@'))
		{
			g1=new grammar;
			g1->n=token[s-_s0-1].s;
			g->l.push_back(g1);
			while(rd(s,",@"))
			{
			g1=new grammar;
			g1->n=token[s-_s0-1].s;
			g->l.push_back(g1);
			}
			if(rd(s,'}'))
				return 1;
		}
	s=t;return 0;
}
int prt(gptr g,int n=0)
{
	int i,j;
	for(j=0;j<n;j++) printf("  ");
	printf("%s\n",g->n.c_str());
	for(i=0;i<g->l.size();i++)
	{
		prt(g->l[i],n+1);
	}
	return 0;
}
int run(char *s,vector <acc::tk> token2,gptr &gm)
{
	token=token2;
	_s0=s;
	int sign=1;
	if(okproc(s,gm)&&(*s)==0)
	printf("Accepted by grammar analysis\n");
	else
	{
		sign=0;
		printf("Not accepted by grammar analysis\n");
		exit(-1);
	}
	if(_out)prt(gm);
	return sign;
}
/////////////////////////////////////////////////////////////////syntax end;
};
typedef syntax::gptr gptr;
string read_all(istream &in)
{
	string res;
	string line;
	while (getline(in, line))
	{
		res+=line;
		res+="\n";
	}
	return res;
}
string trim(string &s)
{
	string res;
	for(int i=0;i<s.length();i++)
	{
		if(s[i]==' '||s[i]=='\t') continue;
		if(s[i]=='<'||s[i]=='>'||s[i]=='"'||s[i]=='"') continue;
		res+=s[i];
	}
	return res;
}
set<string> pragma_once;
string expand_macro(string &s)
{
	string res;
	string line;
	istringstream stream(s);
	while (getline(stream, line))
	{
		string target="#include";
		if(line.length()>=target.size()  &&  strncmp(line.c_str(),target.c_str(),target.size())==0 )
		{

			string filename=line.substr(target.size());
			filename=trim(filename);
			if(pragma_once.find(filename)!=pragma_once.end())
			{
				continue;
			}
			pragma_once.insert(filename);
			string path1=string("")+"./lib.platform/"+g_format_name+"/"+filename;
			string path2="./lib/"+filename;
			ifstream myfile(path1);
			if(myfile.good())
			{
				res+=read_all(myfile);
				res+="\n";
			}
			else
			{
				myfile.open(path2);
				if(myfile.good())
				{
					res+=read_all(myfile);
					res+="\n";
				}
				else
				{
					printf("error: can't find file %s\n",filename.c_str());
					exit(-1);
				}
			}
		}
		else
		{
			res+=line;
			res+="\n";
		}
	}
	return res;
}
char * getcode()
{
	static string s=read_all(cin);
	if(_out)printf("-----------original file----------\n");
	if(_out)printf("%s\n",s.c_str());

	string after;
	int ok=0;
	for(int i=0;i<100;i++)
	{
		after=expand_macro(s);
		if(after==s) {ok=1;s=after;break;}
		s=after;
	}
	if(!ok)
	{
		printf("error: max deepth of expansion exceed\n");
		exit(-1);
	}

	if(_out)printf("-----------after expension----------\n");
	if(_out)printf("%s\n",s.c_str());
	return (char *)s.c_str();
}

struct sytbl
{
	int addr0;
	int count;
	int count2;
	int count3;
	int count4;
	int count5;
	struct type
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
	map<string,type> gv;//全局变量
	//map<string,type> fn;//函数名称
	void ins(string s,type t)
	{
		if(gv.find(s)!=gv.end()) {printf("identifier %s name not unique\n",s.c_str());exit(-1) ;}
		else
		{
			addr0++;
			t.addr=addr0;
			gv.insert(pair<string,type>(s,t));
		}
	}
	void cut_tree(gptr gm)//简化语法树
    {
	int i,j,k;
	vector <gptr>::iterator it,it2;
	for(it=gm->l[1]->l.begin();it!=gm->l[1]->l.end();it++)
		{
			(*it)->l.erase((*it)->l.begin());
			(*it)->l.erase((*it)->l.begin());
			(*it)->l.erase((*it)->l.begin());
			for(i=0;i<(*it)->l[0]->l.size();i++)
			{
					(*it)->l.push_back((*it)->l[0]->l[i]);
			}
			(*it)->l.erase((*it)->l.begin());
		}
		gm->l.erase(gm->l.begin());//删掉变量声明区
		//gm->l[0]=gm->l[0]->l[0];
		for(it=gm->l[0]->l.begin();it!=gm->l[0]->l.end();it++)
		{
			gm->l.push_back(*it);
		}
		gm->l.erase(gm->l.begin());
    }
	void prt()
	{
		map<string,type>::iterator i;
		int j;
		printf("var list:\n");
		for(i=gv.begin();i!=gv.end();i++)
		{
			printf("%s:%s:%d\n",i->first.c_str(),i->second.s.c_str(),i->second.addr);
			for(j=0;j<i->second.para.size();j++)
			{
				printf("->%s\n",i->second.para[j].c_str());
			}
		}
	}
	string creat_tmp(string now)
	{
		count++;
		//addr0++;
		char s[100];
		string r="?t";
		type typetmp;
		typetmp.s="tmp_int";
		typetmp.func=now;
		//typetmp.addr=addr0;
		sprintf(s,"%d",count);
		r+=s;//r+="@";r+=now;
		ins(r,typetmp);
		return r;
	}
	string creat_quo(string now)
	{
		count3++;
		//addr0++;
		char s[100];
		string r="?q";
		type typetmp;
		typetmp.s="quo_int";
		typetmp.func=now;
		sprintf(s,"%d",count3);
		r+=s;//r+="@";r+=now;
		ins(r,typetmp);
		return r;
	}
	string creat_lb()
	{
		count2++;
		//addr0++;
		char s[100];
		string r="?lb";
		type typetmp;
		typetmp.s="temp_label";
		//typetmp.addr=addr0;
		sprintf(s,"%d",count2);
		r+=s;
		ins(r,typetmp);
		return r;
	}
	string creat_str(string text)
	{
		count4++;
		//addr0++;
		char s[100];
		string r="?s";
		type typetmp;
		typetmp.s="string_constant";
		//typetmp.addr=addr0;
		sprintf(s,"%d",count4);
		r+=s;
		typetmp.para.push_back(text);
		ins(r,typetmp);
		return r;
	}
	string creat_arr(vector <string> sl)
	{
		count5++;
		char s[100];
		string r="?arr";
		type typetmp;
		typetmp.s="array_constant";
		//typetmp.addr=addr0;
		sprintf(s,"%d",count5);
		r+=s;
		typetmp.para=sl;
		ins(r,typetmp);
		return r;
	}
	sytbl()
	{
		count=-1;
		count2=-1;
		addr0=-1;
		count3=-1;
		count4=-1;
		count5=-1;
	}
	int ston(string a)
	{
		int r;
		sscanf(a.c_str(),"%d",&r);
		return r;
	}
	string ntos(int a)
	{
		static char tmp[100];
		sprintf(tmp,"%d",a);
		return tmp;
	}
	void makelist(gptr gm)
	{
		vector <gptr>::iterator it,it2;
		type tmptype;
		for(it=gm->l[0]->l.begin();it!=gm->l[0]->l.end();it++)
		{
			tmptype.clear();
			tmptype.s=(*it)->l[0]->n;
				if((*it)->l.size()>1)
					{
						tmptype.s+="_arr";
						int t=1;
						for(int i=1;i<(*it)->l.size();i++)
						{
							t*=ston((*it)->l[i]->n);
						}
						tmptype.n=t;
					}
			ins((*it)->n,tmptype);//加入全局变量表
		}
		for(it=gm->l[1]->l.begin();it!=gm->l[1]->l.end();it++)
		{
			tmptype.clear();
			string func_name=(*it)->n;
			tmptype.s="function_pointer";
			ins((*it)->n,tmptype);
			tmptype.s=(*it)->l[0]->n;
			tmptype.func="";//暂定..
			string stmp="?r";
			ins(stmp+"@"+func_name,tmptype);

			for(it2=(*it)->l[1]->l.begin();it2!=(*it)->l[1]->l.end();it2++)
			{
				    tmptype.clear();
					tmptype.s=(*it2)->l[0]->n;
					if((*it2)->l.size()>1)
					{
						tmptype.s+="_arr";
						int t=1;
						for(int i=1;i<(*it2)->l.size();i++)
						{
							t*=ston((*it2)->l[i]->n);
						}
						tmptype.n=t;
					}
					tmptype.func=func_name;
					ins((*it2)->n+"@"+func_name,tmptype);
					gv.find(func_name)->second.para.push_back((*it2)->n+"@"+func_name);
			}
			for(it2=(*it)->l[2]->l.begin();it2!=(*it)->l[2]->l.end();it2++)
			{
				    tmptype.clear();
				    tmptype.func=func_name;
					tmptype.s=(*it2)->l[0]->n;
					if((*it2)->l.size()>1)
					{
						tmptype.s+="_arr";
						int t=1;
						for(int i=1;i<(*it2)->l.size();i++)
						{
							t*=ston((*it2)->l[i]->n);
						}
						tmptype.n=t;
					}
					ins((*it2)->n+"@"+func_name,tmptype);
			}
		}
	}
	string nowfunc;
	vector <string> lb_goto;
	void deep(gptr gm)
	{
		int i,j,k;
		if(gm->n=="empty") ;
		else if(gm->n=="empty_statement") ;
		else if(gm->n=="parameter") ;
		else if(gm->n[0]>='0'&&gm->n[0]<='9') ;
		else if(gm->n[0]=='\'')  ;
		else if(gm->n.length()>4&&gm->n[0]=='?'&&gm->n[1]=='a') ;
		else if(gm->n=="break");
		else if(gm->n=="continue");
		else if(gm->n=="block_statement"&&gm->l.size()==0) ;
		else if(gm->l.size()==0&&gm->n[0]=='"')
		{
			string t=creat_str(gm->n);
			gm->n=t;
		}
		else if(gm->n=="array_constant")
		{
			vector <string> tmp;
			for(i=0;i<gm->l.size();i++)
			{
				tmp.push_back(gm->l[i]->n);
			}
			string t=creat_arr(tmp);
			gm->n=t;
		}
		else if(gm->l.size()==0)
		{
			string t=nowfunc+"@"+gm->n;
			//printf("[%s]",t.c_str());
			if(gv.find(gm->n+"@"+nowfunc)!=gv.end())
			{
				gm->n=(   gm->n+"@"+nowfunc );
			}
			else if(gv.find(gm->n)!=gv.end())
			{
			}
			else {printf("<can't find identifier:%s>",gm->n.c_str());exit(-1);}
		}
		else if(gm->n=="label")
		{
			type typetmp;
			typetmp.s="label";
			ins(gm->l[0]->n,typetmp);
				return ;
		}
		else if(gm->n=="goto")
		{
			lb_goto.push_back(gm->l[0]->n);

			return ;
		}
		for(i=0;i<gm->l.size();i++)
		{
			deep(gm->l[i]);
		}
	}
	void replace_vb(gptr gm)
	{
		vector <gptr>::iterator it,it2;
		for(it=gm->l.begin();it!=gm->l.end();it++)
		{
			nowfunc=(*it)->n;
			//printf("<<%s>>",nowfunc.c_str());
			for(it2=(*it)->l.begin();it2!=(*it)->l.end();it2++)
			{
				deep(*it2);
			}
		}
		for(int i=0;i<lb_goto.size();i++)
		{
			if(gv.find(lb_goto[i])==gv.end()) {printf("<can't find label: %s>",lb_goto[i].c_str());exit(-1);}
		}
	}
};
//typedef map<string,string>* mptr;
void test(string s)
{
	printf("<%s>",s.c_str());
}
sytbl *_st;
struct quater
{
	struct loop
	{
		string ctn;
		string brk;
	};
	string now_func;
	stack<loop> loopstack;
   // string creat(gptr gm);
	struct sb
	{
		string s1,s2,s3,s4;
	};
	vector <sb> sub;
	string result;
	void clear()
	{
		sub.clear();
	}
	void ins(string s1,string s2,string s3,string s4)
	{
		sb temp;
		temp.s1=s1;temp.s2=s2;temp.s3=s3;temp.s4=s4;
		sub.push_back(temp);
	}
	void prt()
	{
		vector<sb>::iterator it;
		printf("\n");
		printf("Quaternion:\n");
		for(it=sub.begin();it!=sub.end();it++)
		{
			printf(">quat(%s,%s,%s,%s)\n",(it)->s1.c_str(),(it)->s2.c_str(),(it)->s3.c_str(),(it)->s4.c_str());
		}
	}
	string creat_op(gptr gm)
	{
		//printf("<@<%s>>",gm->n.c_str());
		if(gm->n=="+"||gm->n=="-"||gm->n=="*"&&gm->l.size()==2||gm->n=="/"||gm->n=="%"||gm->n==">"||gm->n=="<"||gm->n==">="||gm->n=="<="
		||gm->n=="=="||gm->n=="!="||gm->n=="&&"||gm->n=="||")
		{
			string left=creat_op(gm->l[0]);
			string right=creat_op(gm->l[1]);
			string result=_st->creat_tmp(now_func);
			ins(gm->n,left,right,result);
			return result;
		}
		if(gm->n=="!"||gm->n=="&")
		{
			string left=creat_op(gm->l[0]);
			string right=" ";
			string result=_st->creat_tmp(now_func);
			ins(gm->n,left,right,result);
			return result;
		}
		if(gm->n=="*")
		{
			string left=creat_op(gm->l[0]);
			string right=" ";
			string result=_st->creat_quo(now_func);
			ins(gm->n+"*",left,right,result);
			return result;
		}
		if(gm->n=="[]")
		{
			string left=creat_op(gm->l[0]);
			string right=creat_op(gm->l[1]);
			string result=_st->creat_quo(now_func);
			ins(gm->n,left,right,result);
			return result;
		}
		if(gm->n=="=")
		{
			string left=creat_op(gm->l[0]);
			string right=creat_op(gm->l[1]);;
			string result=left;
			ins(gm->n,left,right,result);
			return result;
		}
		if(gm->l.size()==1)
		{
		//	printf("asdasd");
			if(gm->l[0]->n=="parameter")
				return creat_ft(gm);
		}
		return gm->n;

	}
	string creat_ft(gptr gm)
	{
		vector<gptr>::iterator it;
		vector<string>::iterator it2;
		string ft_func=gm->n;
		string stmp="?r@";
		if(_st->gv.find(ft_func)==_st->gv.end())
		{
			printf("<undefined function: %s>\n",ft_func.c_str());
			exit(-1);
		}
		//printf("<<<函数调用%s 当前函数%s>>>",ft_func.c_str(),now_func.c_str());
		if(gm->l[0]->l.size()!=_st->gv.find(ft_func)->second.para.size())
		{
			//printf("<<%d %d>>",gm->l[0]->l.size(),_st->gv.find(ft_func)->second.para.size());
			printf("<funcion call %s parameter num not match>\n",ft_func.c_str());
			exit(-1);
			return "";
		}
		ins("pushnow"," "," ",now_func);
		for(it=gm->l[0]->l.begin(),it2=_st->gv.find(ft_func)->second.para.begin();it!=gm->l[0]->l.end();it++,it2++)
		{
			string r1=creat_op(*it);
			ins("=",(*it2),r1,(*it2));
		}
		ins("call"," "," ",ft_func);
		ins("popnow"," "," ",now_func);
		return stmp+ft_func;
	}
	string creat_if(gptr gm)
	{
		if(gm->l.size()==3)
		{
			//printf("asdasdasd");
		string lb1=_st->creat_lb();
		string lb2=_st->creat_lb();
		string r1=creat_op(gm->l[0]->l[0]);
		ins("jpz/if-else",r1," ",lb1);
		creat(gm->l[1]);
		ins("jmp/if-else"," "," ",lb2);
		ins("lb/if-else"," "," ",lb1);
		creat(gm->l[2]);
		ins("lb/if-else"," "," ",lb2);
		return "";
		}
		else  if(gm->l.size()==2)
		{
			//printf("asdasdas");
			string lb1=_st->creat_lb();
			string r1=creat_op(gm->l[0]->l[0]);
			ins("jpz/if",r1," ",lb1);
			creat(gm->l[1]);
			ins("lb/if"," "," ",lb1);
			return "";
		}
		else {printf("if parmeter incorrect\n");exit(-1);return "";}
	}
	string creat_label(gptr gm)
	{
		ins("lb"," "," ",gm->l[0]->n);
		return "";
	}
	string creat_goto(gptr gm)
	{
		ins("jmp/goto"," "," ",gm->l[0]->n);
		return "";
	}
	string creat_for(gptr gm)
	{
		loop lptmp;
		string r;
		string lb1=_st->creat_lb();
		string lb2=_st->creat_lb();
		string lb3=_st->creat_lb();
		lptmp.brk=lb3;
		lptmp.ctn=lb2;
		if(gm->l[0]->n!="empty")
		creat_op(gm->l[0]->l[0]);
		ins("jmp/for"," "," ",lb1);
		ins("lb/for"," "," ",lb2);
		if(gm->l[2]->n!="empty")
		creat_op(gm->l[2]->l[0]);
		ins("lb/for"," "," ",lb1);
		if(gm->l[1]->n!="empty")
		 r=creat_op(gm->l[1]->l[0]);
		else r="1";
		ins("jpz/for",r," ",lb3);
		//printf("<!!<<%s>>>",gm->l[3]->n.c_str());

		loopstack.push(lptmp);

		creat(gm->l[3]);
		loopstack.pop();
		ins("jmp/for"," "," ",lb2);
		ins("lb/for"," "," ",lb3);
		//creat_op(gm->l[0]->l[0]);
		return "";
	}
	string creat_break(gptr gm)
	{
		if(!loopstack.empty())
		ins("jmp/break"," "," ",loopstack.top().brk);
		else {printf("incorrect break\n");exit(-1);}
		return "";
	}
	string creat_continue(gptr gm)
	{
		if(!loopstack.empty())
		ins("jmp/continue"," "," ",loopstack.top().ctn);
		else {printf("incorrect continue");exit(-1);}
		return "";
	}
	string creat_return(gptr gm)
	{
		string r=creat_op(gm->l[0]->l[0]);
		string stemp="?r";
		ins("=",stemp+"@"+now_func,r,stemp+"@"+now_func);
		ins("ret/return"," "," "," ");
		return "";
	}
	string creat_asm(gptr gm)
	{
		ins("asm"," "," ",gm->l[0]->n);
		return "";
	}
	string creat(gptr gm)
	{
		vector<gptr>::iterator it;
		//printf("<<<%s>>>",gm->n.c_str());
		string r;
		if(gm->n=="asm_statement") r=creat_asm(gm);
		if(gm->n=="for") r=creat_for(gm);
		if(gm->n=="if_statement") r=creat_if(gm);
		if(gm->n=="goto") r=creat_goto(gm);
		if(gm->n=="label") r=creat_label(gm);
		//if(gm->n=="if_statement") r=creat_if(gm);
		if(gm->n=="break") r=creat_break(gm);
		if(gm->n=="continue") r=creat_continue(gm);
		if(gm->n=="return") r=creat_return(gm);
		if(gm->n=="block_statement")
		{
			for(it=gm->l.begin();it!=gm->l.end();it++)
			{
				r=creat((*it));
			}
		}
		if(gm->n=="expression_statement"||gm->n=="expression") r=creat_op(gm->l[0]);
		//for(it=gm->l.begin();it!=gm->l.end();it++)
	//	{
		//if((*it)->n=="for") {r=creat_for((*it));}
		//if((*it)->n=="if_statement") {r=creat_if((*it));}
		//if((*it)->n=="goto") {r=creat_goto((*it)->l[0]);}
		//if((*it)->n=="label") {r=creat_label((*it)->l[0]);}
		//if((*it)->n=="empty") {r="1";}
		//else if(gm->n=="expression")
		//if((*it)->n=="expression_statement"||(*it)->n=="expression")  {r=creat_op((*it)->l[0]);}
		//printf("%s)",(*it)->n.c_str());
		//}
		return r;
	}
	string func(gptr gm)
	{
		int i;
		now_func=gm->n;
		ins("func"," "," ",gm->n);
		for(i=0;i<gm->l.size();i++)
		creat(gm->l[i]);
		ins("ret/func"," "," ",gm->n);
		return "";
	}
	string proc(gptr gm)
	{
		int i;
		//ins("call"," "," ","main");
		//ins("rts"," "," "," ");
		for(i=0;i<gm->l.size();i++)
		{
			func(gm->l[i]);
		}
		return "";
	}
};
quater *_quat;
map<string,string>* _asmmp;
int prt(gptr g,int n=0)
{
	int i,j;
	for(j=0;j<n;j++) printf("  ");
	printf("%s\n",g->n.c_str());
	/*if(g->n=="quat")
	{
		g->qp->prt(n+1);
	}
	else*/
	for(i=0;i<g->l.size();i++)
	{
		prt(g->l[i],n+1);
	}
	return 0;
}
string cut(string s)
{
	if(s.length()==0) return "";
	std::size_t pos=s.find('/');
	if(s=="/") return "/";
	if(pos==string::npos) return s;
	return s.substr(0,pos);
}
string mod(string s)
{
	if(s=="[ebx]")
		return s;
	if(s.length()>2)
	{
		if(s[0]=='?'&&s[1]=='q')
		{
			string r;
			printf("mov ebx,[%s]\n",s.c_str());
			r="?";
			return "[ebx]";
		}

	}
	if(s.length()>=1)
	{
		if(s[0]=='\''||s[0]>='0'&&s[0]<='9')
		{
			return s;
		}
		else
		{
			string r="[";
			r+="$";
			r+=s;
			r+="]";
			return r;
		}
	}
	return s;
}
int asmcode(map<string,string> asmmp,map<string,sytbl::type> vb,vector<quater::sb> quat)
{
	vector<quater::sb>::iterator it;
	map<string,sytbl::type> ::iterator it2;
	char output_name[]="output.asm";
	FILE* fp=freopen(output_name,"w",stdout);
	if(g_asm_format==format_bin)
	{
		printf(";;format: bin\n\n");
		printf("section .text\n");
		printf("\norg 100h\n");
	}
	else if(g_asm_format==format_elf32)
	{
		printf(";;format: elf32\n\n");
		printf("section .text\n");
		printf("global _start\n");
		printf("_start:\n");
	}
	else if(g_asm_format==format_win32)
	{
		printf(";;format: win32\n\n");
		printf("section .text\n");
		printf("global _main\n");  //gnu ld (native/cygwin/mingw)
		printf("global start\n");  //golink
		printf("_main:\n");
		printf("start:\n");
	}
	printf("call _?db_init\n");
	printf("call main\n");

	if(g_asm_format==format_bin)
	{
		//printf("mov ah,01h\n");
		//printf("int 21h\n");
		printf("mov ah,0x4c\n");
		printf("int 21h\n");
	}
	else if(g_asm_format==format_elf32)
	{
		printf("xor ebx, ebx\n");
		printf("mov eax, 0x1\n");
		printf("int 0x80\n");
	}
	else if(g_asm_format==format_win32)
	{
		printf("ret\n");
	}

	vector <string> arr;
	for(it2=vb.begin();it2!=vb.end();it2++)
	{
		if(it2->second.s=="int_arr"||it2->second.s=="string_constant"||it2->second.s=="array_constant")  arr.push_back(it2->first);
	}
	printf("\n");
	printf("_?db_init:\n");
	for(int i=0;i<arr.size();i++)
	{
		string r="arr?";r+=arr[i];
		printf("mov eax,%s\n",r.c_str());
		printf("mov [%s],eax\n",arr[i].c_str());
	}
	printf("ret\n\n");
	for(it=quat.begin();it!=quat.end();it++)
	{
		if(cut(it->s1)=="call") printf("call %s\n",it->s4.c_str());
		else if(cut(it->s1)=="ret") printf("ret\n");
		else if(cut(it->s1)=="rts") printf("rts\n");
		else if(cut(it->s1)=="func") printf("%s:\n",it->s4.c_str());
		else if(cut(it->s1)=="lb") printf("%s:\n",it->s4.c_str());
		else if(cut(it->s1)=="asm")printf("%s\n",asmmp.find(it->s4)->second.c_str()); //printf("pusha\n%s\npopa\n",asmmp.find(it->s4)->second.c_str());
		else if(cut(it->s1)=="pushnow")
		{
			map<string,sytbl::type>::iterator it3;
			for(it3=vb.begin();it3!=vb.end();it3++)
			{
				if(it3->second.func==it->s4)
				{
					printf("mov eax,[%s]\n",it3->first.c_str());//push,pop的总是变量的内容
					printf("push eax\n");
				}
			}
		}
		else if(cut(it->s1)=="popnow")
		{
			map<string,sytbl::type>::iterator it3;
			for(it3=vb.end();;)
			{
				it3--;
				if(it3->second.func==it->s4)
				{
					printf("pop eax\n");
					printf("mov [%s],eax\n",it3->first.c_str());
				}
				if(it3==vb.begin()) break;
			}
		}
		else if(cut(it->s1)=="+")
		{
			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("add eax,%s\n",mod(it->s3).c_str());
			printf("mov [%s],eax\n",it->s4.c_str());
		}
		else if(cut(it->s1)=="-")
		{
			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("sub eax,%s\n",mod(it->s3).c_str());
			printf("mov [%s],eax\n",it->s4.c_str());
		}
		else if(cut(it->s1)=="*")
		{
			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("mov ecx,%s\n",mod(it->s3).c_str());
			printf("mov edx,0\n");
			printf("imul ecx\n");
			printf("mov [%s],eax\n",it->s4.c_str());
		}
		else if(cut(it->s1)=="/")
		{
			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("mov ecx,%s\n",mod(it->s3).c_str());
			printf("mov edx,0\n");
			printf("idiv ecx\n");
			printf("mov [%s],eax\n",it->s4.c_str());
		}
		else if(cut(it->s1)=="%")
		{
			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("mov ecx,%s\n",mod(it->s3).c_str());
			printf("mov edx,0\n");
			printf("idiv ecx\n");
			printf("mov [%s],edx\n",it->s4.c_str());
		}
		else if(cut(it->s1)=="=")//特殊的运算符
		{
			printf("mov eax,%s\n",mod(it->s3).c_str());
			printf("mov %s,eax\n",mod(it->s2).c_str());
		}
		else if(cut(it->s1)=="!=")
		{
			printf("mov eax,%s\n",mod(it->s3).c_str());
			printf("cmp eax,%s\n",mod(it->s2).c_str());
			printf("LAHF\n");
			printf("SHR EAX,14\n");
			printf("not EAX\n");
			printf("and EAX,1\n");
			printf("mov [%s],eax\n",it->s4.c_str());
		}
		else if(cut(it->s1)=="==")
		{
			printf("mov eax,%s\n",mod(it->s3).c_str());
			printf("cmp eax,%s\n",mod(it->s2).c_str());
			printf("LAHF\n");
			printf("SHR EAX,14\n");
			printf("and EAX,1\n");
			printf("mov [%s],eax\n",it->s4.c_str());
		}
		else if(cut(it->s1)==">")
		{
			printf("xor eax,eax\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("cmp eax,%s\n",mod(it->s3).c_str());
			string lb1=_st->creat_lb();
			printf("jle %s\n",lb1.c_str());
			printf("mov eax,1\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("%s:\n",lb1.c_str());
		}
		else if(cut(it->s1)=="<")
		{
			printf("xor eax,eax\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("cmp eax,%s\n",mod(it->s3).c_str());
			string lb1=_st->creat_lb();
			printf("jge %s\n",lb1.c_str());
			printf("mov eax,1\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("%s:\n",lb1.c_str());
		}
		else if(cut(it->s1)==">=")
		{
			printf("xor eax,eax\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("cmp eax,%s\n",mod(it->s3).c_str());
			string lb1=_st->creat_lb();
			printf("jl %s\n",lb1.c_str());
			printf("mov eax,1\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("%s:\n",lb1.c_str());
		}
		else if(cut(it->s1)=="<=")
		{
			printf("xor eax,eax\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("cmp eax,%s\n",mod(it->s3).c_str());
			string lb1=_st->creat_lb();
			printf("jg %s\n",lb1.c_str());
			printf("mov eax,1\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("%s:\n",lb1.c_str());
		}
		else if(cut(it->s1)=="&&")
		{
			string lb1=_st->creat_lb(),lb2=_st->creat_lb();
			printf("mov eax,1\n");
			printf("mov [%s],eax\n",it->s4.c_str());

			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("cmp eax,0\n");
			printf("jne %s\n",lb1.c_str());
			printf("mov eax,0\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("%s:\n",lb1.c_str());

			printf("mov eax,%s\n",mod(it->s3).c_str());
			printf("cmp eax,0\n");
			printf("jne %s\n",lb2.c_str());
			printf("mov eax,0\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("%s:\n",lb2.c_str());

		}
		else if(cut(it->s1)=="||")
		{
			string lb1=_st->creat_lb(),lb2=_st->creat_lb();
			printf("mov eax,0\n");
			printf("mov [%s],eax\n",it->s4.c_str());

			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("cmp eax,0\n");
			printf("je %s\n",lb1.c_str());
			printf("mov eax,1\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("%s:\n",lb1.c_str());

			printf("mov eax,%s\n",mod(it->s3).c_str());
			printf("cmp eax,0\n");
			printf("je %s\n",lb2.c_str());
			printf("mov eax,1\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("%s:\n",lb2.c_str());
		}
		else if(cut(it->s1)=="!")
		{
			printf("xor eax,eax\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("mov eax,0\n");
			printf("cmp eax,%s\n",mod(it->s2).c_str());
			string lb1=_st->creat_lb();
			printf("jne %s\n",lb1.c_str());
			printf("mov eax,1\n");
			printf("mov [%s],eax\n",it->s4.c_str());
			printf("%s:\n",lb1.c_str());
		}
		else if(cut(it->s1)=="&")
		{
			if(it->s2.length()>2&&it->s2[0]=='?'&&it->s2[1]=='q')//对引用取地址特殊处理
			{
			   printf("mov eax,[%s]\n",it->s2.c_str());
			}
			else printf("mov eax,%s\n",it->s2.c_str());//只能对变量取地址
			printf("mov [%s],eax\n",it->s4.c_str());
		}
		else if(cut(it->s1)=="**")
		{
			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("mov [%s],eax\n",it->s4.c_str());//引用
		}
		else if(cut(it->s1)=="[]")
		{
			printf("mov eax,%s\n",mod(it->s2).c_str());
			//printf("mov ecx,eax\n");
			printf("add eax,%s\n",mod(it->s3).c_str());
			printf("add eax,%s\n",mod(it->s3).c_str());
			printf("add eax,%s\n",mod(it->s3).c_str());
			printf("add eax,%s\n",mod(it->s3).c_str());
			printf("mov [%s],eax\n",it->s4.c_str());
			//printf("mov [%s],eax\n",it->s4.c_str());//引用
		}
		else if(cut(it->s1)=="jpz")
		{
			printf("mov eax,%s\n",mod(it->s2).c_str());
			printf("cmp eax,0\n");
			printf("je %s\n",it->s4.c_str());
		}
		else if(cut(it->s1)=="jmp")
		{
			printf("jmp %s\n",it->s4.c_str());
		}
		else {fprintf(stderr,"<unknow symbol %s>\n",it->s1.c_str());exit(-1);}
	}
	//////////////////////////////////////////////////////////数据区在后面
	printf("\n");
	printf("section .data\n");
	for(it2=vb.begin();it2!=vb.end();it2++)
	{
		if(it2->second.s=="int"||it2->second.s=="tmp_int"||it2->second.s=="quo_int") printf("%s dd 0\n",it2->first.c_str());
		else if(it2->second.s=="int_arr")
		{
			printf("arr?%s times %d dd 0\n",it2->first.c_str(),it2->second.n);
			printf("%s dd 0\n",it2->first.c_str());
			arr.push_back(it2->first);
		}
		else if(it2->second.s=="string_constant")
		{
			printf("arr?%s dd ",it2->first.c_str());
			string t=it2->second.para[0];
			for(int i=1;i<t.length()-1;i++)
			{
				printf("\'%c\',",t[i]);
			}
			printf("0\n");
			printf("%s dd 0\n",it2->first.c_str());
			arr.push_back(it2->first);
		}
		else if(it2->second.s=="array_constant")
		{
			printf("arr?%s dd ",it2->first.c_str());
			for(int i=0;i<it2->second.para.size();i++)
			{

				printf("%s,",it2->second.para[i].c_str());
			}
			printf("0\n");
			printf("%s dd 0\n",it2->first.c_str());
			arr.push_back(it2->first);
		}
	}
	/////////////////////////////////////////////////////////////////////
	fclose(fp);
	//execlp("nasm", "nasm", "-f", "bin","temp.txt","-o","temp.exe" ,(char *)NULL);
	//WinExec(".\\nasm\\nasm.exe  temp.txt -o temp.exe",SW_SHOW);
	//WinExec("d:\\windows\\notepad.exe c:\\wycc.txt",SW_SHOW);
	//getchar();
	/*
	for(;;)
	{
		getchar();
	}*/
	fprintf(stderr,"done\n");
	fprintf(stderr,"format:%s\n",g_format_name.c_str());
	fprintf(stderr,"wrote to \"%s\"\n",output_name);
	return 0;
}
int main(int argc,char *argv[])
{
	//WinExec("C:\\Win-TC\\Win-TC.exe",SW_SHOW);
	//for(;;)
	//{
	if(argc==1) {/*do nothing*/}
	else if(argc==2)
	{
		if(strcmp(argv[1],"bin")==0)
			g_asm_format=format_bin;
		else if(strcmp(argv[1],"elf32")==0)
			g_asm_format=format_elf32;
		else if(strcmp(argv[1],"win32")==0)
			g_asm_format=format_win32;
		else { printf("error:unknown parameter \"%s\"\n(use stdin to input source file)\n",argv[1]); exit(-1);}
	}
	else  {printf("usage:\n./this_program <source.c\n./this_program format <source.c \n\nformat candidates: bin, win32, elf32(default)\n");exit(-1);}

	g_format_name=format_name_table[(int)g_asm_format];

	acc ac;
	syntax syn;
	gptr gm;
	quater quat;
	int i,j,k;
	//i=1;j=2;k=3;
	//(i=j)=k;
	//(i=j)=k;
	char *s=getcode();
	vector <acc::tk> token;
	ac.run(s,s,token);//词法分析输入s,输出到s,token;
	if(_out) printf("result of lexical analysis:%s\n",s);
	if(syn.run(s,token,gm)==0) 1==1;//continue;
    sytbl st;
	st.makelist(gm);
	if(_out)st.prt();
	//prt(gm);
	if(_out) printf("trim grammer tree&&var rename\n");
	st.cut_tree(gm);
	st.replace_vb(gm);
	//st.prt();
	_st=&st;
	//prt(gm);
	//for(i=0;i<gm->l[0]->l.size();i++)
	quat.proc(gm);
	if(_out)quat.prt();
	if(_out)st.prt();
	if(_out) prt(gm);
	asmcode(syn.asm_mp,st.gv,quat.sub);
	//_quat=&quat;
	//_asmmp=&syn.asm_mp;

	//prt(gm);
	//st.prt();
	//printf("表达式转换成四元式:\n");
	//deep(gm);
	//prt(gm);
	//}

	return 0;
}
