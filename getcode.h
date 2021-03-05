#include "comm.h"
struct getcode_t  // get source code from stdin, do macro expansion, return the final code
{
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
	string expand_macro(string &s,asm_format_t asm_format)
	{
		set<string> pragma_once;
		string res;
		string line;
		istringstream stream(s);
		string format_name=format_name_table[asm_format];
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
				string path1=string("")+"./lib.platform/"+format_name+"/"+filename;
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
	string run(asm_format_t asm_format)
	{
		string s=read_all(cin);
		if(_out)printf("-----------original file----------\n");
		if(_out)printf("%s\n",s.c_str());

		string after;
		int ok=0;
		for(int i=0;i<100;i++)
		{
			after=expand_macro(s,asm_format);
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
		return s;
	}
};
