#include "comm.h"
#include "sytbl.h"
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
