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
