#include "comm.h"
struct lexical_analyzer_t
{
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
	vector <token_t> token;

	int run(char *s,cptr &out,vector<token_t> &tok)
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
		token_t ttmp;
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
