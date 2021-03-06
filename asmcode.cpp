#include "comm.h"
struct asm_gen_t
{
	FILE *fp;
	int label_count;
	asm_gen_t()
	{
		fp=0;
		label_count=-1;
	}
	void write_asm( const char * format, ... )
	{
		va_list args;
		va_start (args, format);
		vfprintf (fp,format, args);
		va_end (args);
	}
	string cut(string s)
	{
		if(s.length()==0) return "";
		std::size_t pos=s.find('/');
		if(s=="/") return "/";
		if(pos==string::npos) return s;
		return s.substr(0,pos);
	}
	string mod(string s) //generate operand
	{
		if(s=="[ebx]")
			return s;
		if(s.length()>2)
		{
			if(s[0]=='?'&&s[1]=='q')//it's a variable related to pointer
			{
				string r;
				write_asm("mov ebx,[%s]\n",s.c_str());   //indirect addressing
				r="?";
				return "[ebx]"; //another indirect addressing
			}

		}
		if(s.length()>=1)
		{
			if(s[0]=='\''||s[0]>='0'&&s[0]<='9')  //it's integer or char
			{
				return s;
			}
			else//it's a normal variable
			{
				string r="[";
				r+="$";
				r+=s;
				r+="]";
				return r;   //only one indirect addressing
			}
		}
		return s;
	}
	string creat_lb()
	{
		label_count++;
		//addr0++;
		char s[100];
		string r = "?LB"; // LB for label generated by asm_gen, lb for quat_gen
		var_type_t typetmp;
		typetmp.s = "temp_label";
		//typetmp.addr=addr0;
		sprintf(s, "%d", label_count);
		r += s;
		return r;
	}

	int run(map<string,string> asmmp,map<string,var_type_t> vb,vector<quat_t> quat,asm_format_t asm_format,string output_name)
	{
		vector<quat_t>::iterator it;
		map<string,var_type_t> ::iterator it2;
		//char output_name[]="output.asm";
		fp=fopen(output_name.c_str(),"w");
		if(asm_format==format_bin)
		{
			write_asm(";;format: bin\n\n");
			write_asm("section .text\n");
			write_asm("\norg 100h\n");
		}
		else if(asm_format==format_elf32)
		{
			write_asm(";;format: elf32\n\n");
			write_asm("section .text\n");
			write_asm("global _start\n");
			write_asm("_start:\n");
		}
		else if(asm_format==format_win32)
		{
			write_asm(";;format: win32\n\n");
			write_asm("section .text\n");
			write_asm("global _main\n");  //gnu ld (native/cygwin/mingw)
			write_asm("global start\n");  //golink
			write_asm("_main:\n");
			write_asm("start:\n");
		}
		write_asm("call _?db_init\n");
		write_asm("call main\n");

		if(asm_format==format_bin)
		{
			//write_asm("mov ah,01h\n");
			//write_asm("int 21h\n");
			write_asm("mov ah,0x4c\n");
			write_asm("int 21h\n");
		}
		else if(asm_format==format_elf32)
		{
			write_asm("xor ebx, ebx\n");
			write_asm("mov eax, 0x1\n");
			write_asm("int 0x80\n");
		}
		else if(asm_format==format_win32)
		{
			write_asm("ret\n");
		}

		vector <string> arr;
		for(it2=vb.begin();it2!=vb.end();it2++)
		{
			if(it2->second.s=="int_arr"||it2->second.s=="string_constant"||it2->second.s=="array_constant")  arr.push_back(it2->first);
		}
		write_asm("\n");
		write_asm("_?db_init:\n");
		for(int i=0;i<arr.size();i++)
		{
			string r="arr?";r+=arr[i];
			write_asm("mov eax,%s\n",r.c_str());
			write_asm("mov [%s],eax\n",arr[i].c_str());
		}
		write_asm("ret\n\n");
		for(it=quat.begin();it!=quat.end();it++)
		{
			if(cut(it->s1)=="call") write_asm("call %s\n",it->s4.c_str());
			else if(cut(it->s1)=="ret") write_asm("ret\n");
			else if(cut(it->s1)=="rts") write_asm("rts\n");
			else if(cut(it->s1)=="func") write_asm("%s:\n",it->s4.c_str());
			else if(cut(it->s1)=="lb") write_asm("%s:\n",it->s4.c_str());
			else if(cut(it->s1)=="asm")write_asm("%s\n",asmmp.find(it->s4)->second.c_str()); //write_asm("pusha\n%s\npopa\n",asmmp.find(it->s4)->second.c_str());
			else if(cut(it->s1)=="pushnow")
			{
				map<string,var_type_t>::iterator it3;
				for(it3=vb.begin();it3!=vb.end();it3++)
				{
					if(it3->second.func==it->s4)
					{
						write_asm("mov eax,[%s]\n",it3->first.c_str());//push,pop的总是变量的内容
						write_asm("push eax\n");
					}
				}
			}
			else if(cut(it->s1)=="popnow")
			{
				map<string,var_type_t>::iterator it3;
				for(it3=vb.end();;)
				{
					it3--;
					if(it3->second.func==it->s4)
					{
						write_asm("pop eax\n");
						write_asm("mov [%s],eax\n",it3->first.c_str());
					}
					if(it3==vb.begin()) break;
				}
			}
			else if(cut(it->s1)=="+")
			{
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("add eax,%s\n",mod(it->s3).c_str());
				write_asm("mov [%s],eax\n",it->s4.c_str());
			}
			else if(cut(it->s1)=="-")
			{
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("sub eax,%s\n",mod(it->s3).c_str());
				write_asm("mov [%s],eax\n",it->s4.c_str());
			}
			else if(cut(it->s1)=="*")
			{
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("mov ecx,%s\n",mod(it->s3).c_str());
				write_asm("mov edx,0\n");
				write_asm("imul ecx\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
			}
			else if(cut(it->s1)=="/")
			{
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("mov ecx,%s\n",mod(it->s3).c_str());
				write_asm("mov edx,0\n");
				write_asm("idiv ecx\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
			}
			else if(cut(it->s1)=="%")
			{
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("mov ecx,%s\n",mod(it->s3).c_str());
				write_asm("mov edx,0\n");
				write_asm("idiv ecx\n");
				write_asm("mov [%s],edx\n",it->s4.c_str());
			}
			else if(cut(it->s1)=="=")//特殊的运算符
			{
				write_asm("mov eax,%s\n",mod(it->s3).c_str());
				write_asm("mov %s,eax\n",mod(it->s2).c_str());
			}
			else if(cut(it->s1)=="!=")
			{
				write_asm("mov eax,%s\n",mod(it->s3).c_str());
				write_asm("cmp eax,%s\n",mod(it->s2).c_str());
				write_asm("LAHF\n");
				write_asm("SHR EAX,14\n");
				write_asm("not EAX\n");
				write_asm("and EAX,1\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
			}
			else if(cut(it->s1)=="==")
			{
				write_asm("mov eax,%s\n",mod(it->s3).c_str());
				write_asm("cmp eax,%s\n",mod(it->s2).c_str());
				write_asm("LAHF\n");
				write_asm("SHR EAX,14\n");
				write_asm("and EAX,1\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
			}
			else if(cut(it->s1)==">")
			{
				write_asm("xor eax,eax\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("cmp eax,%s\n",mod(it->s3).c_str());
				string lb1=creat_lb();
				write_asm("jle %s\n",lb1.c_str());
				write_asm("mov eax,1\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("%s:\n",lb1.c_str());
			}
			else if(cut(it->s1)=="<")
			{
				write_asm("xor eax,eax\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("cmp eax,%s\n",mod(it->s3).c_str());
				string lb1=creat_lb();
				write_asm("jge %s\n",lb1.c_str());
				write_asm("mov eax,1\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("%s:\n",lb1.c_str());
			}
			else if(cut(it->s1)==">=")
			{
				write_asm("xor eax,eax\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("cmp eax,%s\n",mod(it->s3).c_str());
				string lb1=creat_lb();
				write_asm("jl %s\n",lb1.c_str());
				write_asm("mov eax,1\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("%s:\n",lb1.c_str());
			}
			else if(cut(it->s1)=="<=")
			{
				write_asm("xor eax,eax\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("cmp eax,%s\n",mod(it->s3).c_str());
				string lb1=creat_lb();
				write_asm("jg %s\n",lb1.c_str());
				write_asm("mov eax,1\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("%s:\n",lb1.c_str());
			}
			else if(cut(it->s1)=="&&")
			{
				string lb1=creat_lb(),lb2=creat_lb();
				write_asm("mov eax,1\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());

				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("cmp eax,0\n");
				write_asm("jne %s\n",lb1.c_str());
				write_asm("mov eax,0\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("%s:\n",lb1.c_str());

				write_asm("mov eax,%s\n",mod(it->s3).c_str());
				write_asm("cmp eax,0\n");
				write_asm("jne %s\n",lb2.c_str());
				write_asm("mov eax,0\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("%s:\n",lb2.c_str());

			}
			else if(cut(it->s1)=="||")
			{
				string lb1=creat_lb(),lb2=creat_lb();
				write_asm("mov eax,0\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());

				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("cmp eax,0\n");
				write_asm("je %s\n",lb1.c_str());
				write_asm("mov eax,1\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("%s:\n",lb1.c_str());

				write_asm("mov eax,%s\n",mod(it->s3).c_str());
				write_asm("cmp eax,0\n");
				write_asm("je %s\n",lb2.c_str());
				write_asm("mov eax,1\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("%s:\n",lb2.c_str());
			}
			else if(cut(it->s1)=="!")
			{
				write_asm("xor eax,eax\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("mov eax,0\n");
				write_asm("cmp eax,%s\n",mod(it->s2).c_str());
				string lb1=creat_lb();
				write_asm("jne %s\n",lb1.c_str());
				write_asm("mov eax,1\n");
				write_asm("mov [%s],eax\n",it->s4.c_str());
				write_asm("%s:\n",lb1.c_str());
			}
			else if(cut(it->s1)=="&")
			{
				if(it->s2.length()>2&&it->s2[0]=='?'&&it->s2[1]=='q')//对引用取地址特殊处理
				{
					write_asm("mov eax,[%s]\n",it->s2.c_str());
				}
				else write_asm("mov eax,%s\n",it->s2.c_str());//只能对变量取地址
				write_asm("mov [%s],eax\n",it->s4.c_str());
			}
			else if(cut(it->s1)=="**")
			{
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("mov [%s],eax\n",it->s4.c_str());//引用
			}
			else if(cut(it->s1)=="[]")
			{
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				//write_asm("mov ecx,eax\n");
				write_asm("add eax,%s\n",mod(it->s3).c_str());
				write_asm("add eax,%s\n",mod(it->s3).c_str());
				write_asm("add eax,%s\n",mod(it->s3).c_str());
				write_asm("add eax,%s\n",mod(it->s3).c_str());
				write_asm("mov [%s],eax\n",it->s4.c_str());
				//write_asm("mov [%s],eax\n",it->s4.c_str());//引用
			}
			else if(cut(it->s1)=="jpz")
			{
				write_asm("mov eax,%s\n",mod(it->s2).c_str());
				write_asm("cmp eax,0\n");
				write_asm("je %s\n",it->s4.c_str());
			}
			else if(cut(it->s1)=="jmp")
			{
				write_asm("jmp %s\n",it->s4.c_str());
			}
			else {printf("<unknow symbol %s>\n",it->s1.c_str());exit(-1);}
		}
		//////////////////////////////////////////////////////////数据区在后面
		write_asm("\n");
		write_asm("section .data\n");
		for(it2=vb.begin();it2!=vb.end();it2++)
		{
			if(it2->second.s=="int"||it2->second.s=="tmp_int"||it2->second.s=="quo_int") write_asm("%s dd 0\n",it2->first.c_str());
			else if(it2->second.s=="int_arr")
			{
				write_asm("arr?%s times %d dd 0\n",it2->first.c_str(),it2->second.n);
				write_asm("%s dd 0\n",it2->first.c_str());
				arr.push_back(it2->first);
			}
			else if(it2->second.s=="string_constant")
			{
				write_asm("arr?%s dd ",it2->first.c_str());
				string t=it2->second.para[0];
				for(int i=1;i<t.length()-1;i++)
				{
					write_asm("\'%c\',",t[i]);
				}
				write_asm("0\n");
				write_asm("%s dd 0\n",it2->first.c_str());
				arr.push_back(it2->first);
			}
			else if(it2->second.s=="array_constant")
			{
				write_asm("arr?%s dd ",it2->first.c_str());
				for(int i=0;i<it2->second.para.size();i++)
				{

					write_asm("%s,",it2->second.para[i].c_str());
				}
				write_asm("0\n");
				write_asm("%s dd 0\n",it2->first.c_str());
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
		printf("done\n");
		printf("format:%s\n",format_name_table[asm_format]);
		printf("wrote to \"%s\"\n",output_name.c_str());
		return 0;
	}
};
int asm_code_gen(const map<string,string> &asmmp,const map<string,var_type_t> &vb,const vector<quat_t> &quat,asm_format_t asm_format,string output_name)
{
	asm_gen_t asm_gen;
	return asm_gen.run(asmmp,vb,quat,asm_format,output_name);
}