//#include <windows.h>
//#include <unistd.h>
#include "comm.h"
#include "acc.h"
#include "syntax.h"
#include "misc.h"
#include "sytbl.h"
#include "quater.h"
#include "asmcode.h"
using namespace std;

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
