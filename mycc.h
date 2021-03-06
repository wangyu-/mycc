#pragma once
#include "comm.h"
string getcode(asm_format_t asm_format);
int lexical_analyze(char *s,cptr &out,vector<token_t> &tok);
int syntax_analyze(char *s,const vector <token_t> &token_in,gptr &gm, map<string,string> &asm_mp);
int intermd_gen(gptr grammer_tree,map<string,var_type_t> &gv,vector <quat_t> &quat);
int asm_code_gen(const map<string,string> &asmmp,const map<string,var_type_t> &vb,const vector<quat_t> &quat,asm_format_t asm_format,string output_name);
