#pragma once
#include "comm.h"
struct symbol_table_t
{
	int addr0;
	int count;
	int count2;
	int count3;
	int count4;
	int count5;
	map<string,var_type_t> gv;//全局变量
	//map<string,type> fn;//函数名称
	void ins(string s,var_type_t t)
	{
		if(gv.find(s)!=gv.end()) {printf("identifier %s name not unique\n",s.c_str());exit(-1) ;}
		else
		{
			addr0++;
			t.addr=addr0;
			gv.insert(pair<string,var_type_t>(s,t));
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
		map<string,var_type_t>::iterator i;
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
		var_type_t typetmp;
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
		var_type_t typetmp;
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
		var_type_t typetmp;
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
		var_type_t typetmp;
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
		var_type_t typetmp;
		typetmp.s="array_constant";
		//typetmp.addr=addr0;
		sprintf(s,"%d",count5);
		r+=s;
		typetmp.para=sl;
		ins(r,typetmp);
		return r;
	}
	symbol_table_t()
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
		var_type_t tmptype;
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
			var_type_t typetmp;
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
