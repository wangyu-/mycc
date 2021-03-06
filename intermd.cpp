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

struct quat_gen_t
{
	symbol_table_t *_st;
	struct loop
	{
		string ctn;
		string brk;
	};
	string now_func;
	stack<loop> loopstack;
	// string creat(gptr gm);
	vector <quat_t> quat;
	string result;
	void clear()
	{
		quat.clear();
	}
	void ins(string s1,string s2,string s3,string s4)
	{
		quat_t temp;
		temp.s1=s1;temp.s2=s2;temp.s3=s3;temp.s4=s4;
		quat.push_back(temp);
	}
	void prt()
	{
		vector<quat_t>::iterator it;
		printf("\n");
		printf("Quaternion:\n");
		for(it=quat.begin();it!=quat.end();it++)
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
	string proc(gptr gm,symbol_table_t &st)
	{
		_st=&st;
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

int intermd_gen(gptr grammer_tree,map<string,var_type_t> &gv,vector <quat_t> &quat)
{
	symbol_table_t symbol_table;
	symbol_table.makelist(grammer_tree);
	if(_out)symbol_table.prt();
	//if(_out) printf("trim grammer tree&&var rename\n");
	symbol_table.cut_tree(grammer_tree); //trim grammer tree, remove the variable declaritions 
	symbol_table.replace_vb(grammer_tree);//replace variales names to unique ones

	quat_gen_t quat_gen; //intermediate code generator
	quat_gen.proc(grammer_tree,symbol_table);  //generate quaternions from grammer_tree and symbol_table

	quat=quat_gen.quat;
	gv=symbol_table.gv;

	if(_out)quat_gen.prt();
	if(_out)symbol_table.prt();
	if(_out)grammer_tree->prt();
}
