//#include <windows.h>
//#include <unistd.h>
#include "comm.h"
#include "lex.h"
#include "syntax.h"
#include "getcode.h"
#include "sytbl.h"
#include "quat_gen.h"
#include "asmcode.h"
using namespace std;

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
		else { printf("error:unknown parameter \"%s\"\n(use stdin to input source file)\n",argv[1]); exit(-1);}
	}
	else  {printf("usage:\n./this_program <source.c\n./this_program format <source.c \n\nformat candidates: bin, win32, elf32(default)\n");exit(-1);}

	lexical_analyzer_t lexical_analyzer;
	syntax_analyzer_t syntax_analyzer;
	gptr grammer_tree;
	quat_gen_t quat_gen; //intermediate code generator

	getcode_t getcode;  //get input code, return code after macro expansion

	string code=getcode.run(asm_format);
	char *code_s=(char*)code.c_str();
	vector <token_t> token;//token
	char *token_s;//char stream of token
	lexical_analyzer.run(code_s,token_s,token);//lexical analysis, generate token_s and token
	if(_out) printf("result of lexical analysis:%s\n",token_s);
	syntax_analyzer.run(token_s,token,grammer_tree);
	symbol_table_t symbol_table;
	symbol_table.makelist(grammer_tree);
	if(_out)symbol_table.prt();
	//if(_out) printf("trim grammer tree&&var rename\n");
	symbol_table.cut_tree(grammer_tree); //trim grammer tree, remove the variable declaritions 
	symbol_table.replace_vb(grammer_tree);//replace variales names to unique ones

	quat_gen.proc(grammer_tree,symbol_table);  //generate quaternions from grammer_tree and symbol_table
	if(_out)quat_gen.prt();
	if(_out)symbol_table.prt();
	if(_out) grammer_tree->prt();
	asm_gen_t asmcode; // asm code generator
	string output_name="output.asm";
	asmcode.run(syntax_analyzer.asm_mp,symbol_table.gv,quat_gen.quat,asm_format,output_name); //generate asm code from asm_map, global_variable_table, and quaternions

	return 0;
}
