//#include <windows.h>
//#include <unistd.h>
#include "comm.h"
#include "mycc.h"
using namespace std;

void print_help()
{
	printf("usage:\n./this_program <source.c\n./this_program format <source.c \n\nformat candidates: bin, win32, elf32(default)\n");
}
int main(int argc,char *argv[])
{
	//WinExec("C:\\Win-TC\\Win-TC.exe",SW_SHOW);
	//for(;;)
	//{
	asm_format_t asm_format=format_elf32;

	if(argc==1) {/*do nothing*/}
	else if(argc==2)
	{
		if(strcmp(argv[1],"bin")==0)
			asm_format=format_bin;
		else if(strcmp(argv[1],"elf32")==0)
			asm_format=format_elf32;
		else if(strcmp(argv[1],"win32")==0)
			asm_format=format_win32;
		else { printf("error:unknown parameter \"%s\"\n\n",argv[1]);print_help();exit(-1);}
	}
	else  {print_help();exit(-1);}

	//====step1 get code====
	string code=getcode(asm_format);
	char *code_s=(char*)code.c_str();
	//====step2 lexical analyze====
	vector <token_t> token;//token
	char *token_s;//char stream of token
	lexical_analyze(code_s,token_s,token);//lexical analysis, generate token_s and token
	if(_out) printf("result of lexical analysis:%s\n",token_s);
	//====step3 syntax_analyze====
	map<string,string> asm_table;   //table of embedded asm code
	gptr grammer_tree;              //grammer tree
	syntax_analyze(token_s,token,grammer_tree,asm_table);
	//====step4 gen intermediate representation====
	map<string,var_type_t> var_table;   //variable table
	vector <quat_t> quaternions;       //quaternions representation of code
	intermd_gen(grammer_tree,var_table,quaternions);
	//====step5 gen asm code====
	asm_code_gen(asm_table,var_table,quaternions,asm_format,"output.asm");

	return 0;
}
