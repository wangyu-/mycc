#include "comm.h"
struct syntax_analyzer_t
{
	vector <token_t> token; //token from lexical_analyzer
	char * _s0; //original char begining for input s
	int asm0;  //counter to generate name for asm block
	map<string,string> asm_mp;  //table for embedded asm code

	syntax_analyzer_t()
	{
		asm0=-1;
	}

	string gen_asm()
	{
		asm0++;
		char tmp[100];
		sprintf(tmp,"%d",asm0);
		string r="?asm";
		return r+tmp;
	}
	///////////////////////////////
	int rd(cptr &s,char c)
	{
		if(*s==c) {s+=1;return 1;}
		return 0;
	}
	int rd(cptr &s,char *c,int &t)
	{
		int l=strlen(c);
		if(memcmp(s,c,l)==0)  {s+=l;t=l;return 1;}
		return 0;
	}
	int rd(cptr &s,char *c,string &t)
	{
		int l=strlen(c);
		t=c;
		if(memcmp(s,c,l)==0)  {s+=l;return 1;}
		return 0;
	}
	int rd(cptr &s,char *c)
	{
		int l=strlen(c);
		if(memcmp(s,c,l)==0)  {s+=l;return 1;}
		return 0;
	}
	int okop/*表达式语句*/(cptr &s,gptr &g) //expression_statement
	{
		g=new grammar_t;
		gptr gt;
		g->n="expression_statement";
		cptr t=s;
		if(okS(s,gt))
			if( rd(s,';') )
			{
				g->l.push_back(gt);
				return 1;
			}
		s=t;return 0;
	}
	int tb/*变量声明区*/(cptr &s,gptr &g) //variable declare zone
	{
		g=new grammar_t;
		g->n="var_declare_zone";
		gptr gt;
		while(okts(s,gt)) {g->l.push_back(gt);}
		return 1;
	}
	int fb/*函数区*/(cptr &s,gptr &g) // function zone
	{
		g=new grammar_t;
		g->n="function_zone";
		gptr gt;
		while(okfs(s,gt)) {g->l.push_back(gt);}
		return 1;
	}
	int okif/*if语句*/(cptr &s,gptr &g) //if statement
	{
		cptr t=s;
		g=new grammar_t;gptr gt;
		g->n="if_statement";
		if(rd(s,'i'))
			if(rd(s,'('))
				if(okS(s,gt))
				{
					gptr g2=new grammar_t;g2->n="expression";g->l.push_back(g2);g2->l.push_back(gt);
					//g->l.push_back(gt);
					//运算语句
					if(rd(s,')'))
						if(oks(s,gt))
						{
							g->l.push_back(gt);
							cptr t2=s;
							if(rd(s,'e'))
								if(oks(s,gt))
								{
									g->l.push_back(gt);
									return 1;
								}
							s=t2;return 1;
						}
				}
		s=t;return 0;
	}
	int okfor/*for语句*/(cptr &s,gptr &g) // for statement
	{
		cptr t=s;
		gptr gt;
		g=new grammar_t;
		g->n="for";
		if(rd(s,"f("))
		{
			if(okS(s,gt)){gptr g2=new grammar_t;g2->n="expression";g->l.push_back(g2);g2->l.push_back(gt);}
			else {gptr g2=new grammar_t;g2->n="empty";g->l.push_back(g2);}
			if(rd(s,';'))
			{
				if(okS(s,gt)) {gptr g2=new grammar_t;g2->n="expression";g->l.push_back(g2);g2->l.push_back(gt);}
				else {gptr g2=new grammar_t;g2->n="empty";g->l.push_back(g2);}
				if(rd(s,';'))
				{
					if(okS(s,gt)) {gptr g2=new grammar_t;g2->n="expression";g->l.push_back(g2);g2->l.push_back(gt);}
					else {gptr g2=new grammar_t;g2->n="empty";g->l.push_back(g2);}
					if(rd(s,')'))
					{
						if(oks(s,gt)){g->l.push_back(gt); return 1;}
					}
				}
			}
		}
		s=t;return 0;
	}
	int okms(cptr &s,gptr &g) //code block
	{
		cptr t=s;
		g=new grammar_t;
		g->n="block_statement";
		gptr gt;
		if(rd(s,'{'))
		{
			while(oks(s,gt)) g->l.push_back(gt);
			if(rd(s,'}')) return 1;
		}
		s=t;return 0;
	}
	int okre(cptr &s,gptr &g)//return
	{
		cptr t=s;
		gptr gt;
		g=new grammar_t;
		g->n="return";
		if(rd(s,'r'))
		{

			if(okS(s,gt))
			{
				gptr g2=new grammar_t;
				g2->n="expression";
				g2->l.push_back(gt);
				g->l.push_back(g2);
				if(rd(s,';'))
					return 1;
			}
		}
		s=t;
		return 0;
	}
	int okctn(cptr &s,gptr &g)//continue
	{
		cptr t=s;
		gptr gt;
		g=new grammar_t;
		g->n="continue";
		if(rd(s,'c'))
		{
			if(rd(s,';'))
				return 1;
		}
		s=t;
		return 0;
	}
	int okbrk(cptr &s,gptr &g)//break
	{
		cptr t=s;
		gptr gt;
		g=new grammar_t;
		g->n="break";
		if(rd(s,'b'))
		{
			if(rd(s,';'))
				return 1;
		}
		s=t;
		return 0;
	}
	int oklb(cptr &s,gptr &g)//label
	{
		cptr t=s;
		g=new grammar_t;
		g->n="label";
		gptr g1;
		g1=new grammar_t;
		// g1->n=token[s-_s0].s; //原位置 有bug
		//g->l.push_back(g1);
		if(rd(s,'w'))
			if(rd(s,':'))
			{
				g1->n=token[s-_s0-1-1].s;
				g->l.push_back(g1);
				return 1;
			}
		s=t;return 0;
	}
	int okgt(cptr &s,gptr &g)//goto
	{
		cptr t=s;
		g=new grammar_t;
		g->n="goto";
		gptr g1;
		g1=new grammar_t;

		if(rd(s,'g'))
		{
			g1->n=token[s-_s0].s;
			g->l.push_back(g1);
			if(rd(s,'w'))
			{
				if(rd(s,';'))
					return 1;
			}

		}
		s=t;return 0;
	}
	int okasm(cptr &s,gptr &g)//embed asm
	{
		cptr t=s;
		g=new grammar_t;
		g->n="asm_statement";
		gptr g2;
		if(rd(s,'#'))
		{
			string t1=gen_asm();
			g2=new grammar_t;
			g2->n=t1;
			g->l.push_back(g2);
			string t2=token[s-_s0-1].s;
			asm_mp.insert(pair<string,string>(t1,t2));
			return 1;
		}
		s=t;return 0;
	}
	int oks(cptr &s,gptr &g)//statement
	{
		cptr t=s;
		g=new grammar_t;
		g->n="undecided_statement";
		gptr gt;
		if(okasm(s,gt)){g=gt;return 1;}
		if(oklb(s,gt)) {g=gt;return 1;}
		if(okgt(s,gt)) {g=gt;return 1;}
		if(okif(s,gt)) {g=gt;return 1;}//if语句
		if(okfor(s,gt)){g=gt;return 1;}//for语句

		if(okop(s,gt)) {g=gt;return 1;}
		if(okms(s,gt)) {g=gt;return 1;}
		if(okre(s,gt)) {g=gt;return 1;}
		if(okctn(s,gt)){g=gt;return 1;}
		if(okbrk(s,gt)){g=gt;return 1;}
		if(rd(s,';')){g=new grammar_t;g->n="empty_statement";return 1;}
		s=t;
		return 0;
	}
	int sb/*语句区*/(cptr &s,gptr &g) //statement zone
	{

		cptr t=s;
		gptr gt;
		g=new grammar_t;
		g->n="statement_zone";
		while(oks(s,gt)) g->l.push_back(gt);

		return 1;
	}
	int okproc/*程序*/(cptr &s,gptr &g) //the whole program
	{
		cptr t=s;
		gptr gt;
		g=new grammar_t;
		g->n="program";
		//line1:
		//	int sign=0;
		if(tb(s,gt)) {g->l.push_back(gt);}
		if(fb(s,gt)) {g->l.push_back(gt);}
line1:
		int sign=0;
		if(tb(s,gt))
		{
			if(gt->l.size())sign=1;
			for(int i=0;i<gt->l.size();i++)
				g->l[0]->l.push_back(gt->l[i]);
		}
		if(fb(s,gt))
		{
			if(gt->l.size())sign=1;
			for(int i=0;i<gt->l.size();i++)
				g->l[1]->l.push_back(gt->l[i]);
		}
		if(sign) goto line1;

		//if(okft(s,gt)) g->l.push_back(gt);
		return 1;
	}
	int okfe/*变量声明*/(cptr &s,gptr &g) //variable declare zone
	{
		cptr t=s;
		gptr gt,gt1;
		if(okt(s,gt))
		{
			gt1=gt;
			if(okse(s,gt))
			{
				g=gt;g->l.push_back(gt1);
				while(rd(s,"[@]"))
				{
					gt1=new grammar_t;
					gt1->n=token[s-_s0-2].s;
					g->l.push_back(gt1);
				}
				return 1;
			}
		}
		s=t;g=0;return 0;
	}
	int okfl/*函数参数表*/(cptr &s,gptr &g) //function parameter list
	{
		cptr t=s;
		gptr gt;
		g=new grammar_t;
		g->n="parameter_list";
		if(okfe(s,gt))
		{
			g->l.push_back(gt);
			while(rd(s,','))
			{
				if(okfe(s,gt)){g->l.push_back(gt);continue;}
				s-=1;break;
			}
			return 1;
		}
		s=t;return 1;//一定能接受
	}
	int okfs/*函数*/(cptr &s,gptr &g) //function
	{
		cptr t=s;
		g=new grammar_t;gptr gt;
		//g->n="function";
		if(okt(s,gt))
		{
			g->l.push_back(gt);
			if(rd(s,'w'))
			{
				g->n=token[s-_s0-1].s;
				if(rd(s,'('))
					if(okfl(s,gt))
					{
						g->l.push_back(gt);
						if(rd(s,')'))
						{
							if(rd(s,'{'))
							{
								if(tb(s,gt)) g->l.push_back(gt);
								if(sb(s,gt)) g->l.push_back(gt);
								if(rd(s,'}'))
									return 1;
							}
						}
					}
			}
		}
		s=t;return 0;
	}
	int okt/*数据类型*/(cptr &s,gptr &g) //data type
	{
		cptr t=s;
		gptr gt;
		g=new grammar_t;
		if(rd(s,'l')){while(rd(s,'*')); g->n="int";return 1;}//忽略掉所有*
		s=t;return 0;
	}
	int okse/*变量名*/(cptr &s,gptr &g)//variable name
	{
		cptr t=s;
		gptr gt;
		g=new grammar_t;
		g->n=token[int(s-_s0)].s;
		if(rd(s,'w')) {return 1;}
		s=t;return 0;
	}
	int okts/*变量声明*/(cptr &s,gptr &g)//variable declare
	{
		cptr t=s;
		gptr gt,gt1;
		if(okt(s,gt))
		{
			gt1=gt;
			if(okse(s,gt))
			{
				g=gt;g->l.push_back(gt1);
				while(rd(s,"[@]"))
				{
					gt1=new grammar_t;
					gt1->n=token[s-_s0-2].s;
					g->l.push_back(gt1);
				}
				if(rd(s,';')){ return 1;}
			}
		}
		s=t;g=0;return 0;
	}


	int okft/*函数调用*/(cptr &s,gptr &g)//function call
	{
		cptr t=s;
		gptr gt;
		g=new grammar_t;
		if(rd(s,'w'))
		{
			g->n=token[s-_s0-1].s;
			if(rd(s,'('))
				if(okftl(s,gt))//参数表
				{
					g->l.push_back(gt);
					if(rd(s,')'))
						return 1;
				}
		}
		s=t;return 0;
	}
	int okftl/*函数调用参数表*/(cptr &s,gptr &g)//function call parameter list
	{
		cptr t=s;
		gptr gt;
		g=new grammar_t;
		g->n="parameter";
		if(okS(s,gt))
		{
			g->l.push_back(gt);
			while(rd(s,','))
			{
				if(okS(s,gt)) {g->l.push_back(gt);continue;}//函数调用是表达式
				s-=1;break;
			}
			return 1;
		}
		g->c();g->n="parameter";s=t;return 1;//肯定会接受
	}


	int okS/*&&||*/(cptr &s,gptr &g)
	{
		cptr t=s;
		gptr gt,g1,g2;
		string o;
		if(okA(s,gt))
		{
			gptr g1=gt;
			while(rd(s,"||",o)||rd(s,"&&",o))
			{
				if(okA(s,gt))
				{
					g2=new grammar_t;
					g2->n=o;
					g2->l.push_back(g1);
					g2->l.push_back(gt);
					g1=g2;
					continue;
				}
				s-=o.length();break;
			}
			g=g1;
			return 1;
		}
		s=t;return 0;
	}
	int okA_notused/*=*/(cptr &s,gptr &g)  //left associate version
	{
		cptr t=s;
		gptr gt,g1,g2;
		string o;
		if(okE(s,gt))
		{
			gptr g1=gt;
			while(rd(s,"=",o))
			{
				if(okE(s,gt))
				{
					g2=new grammar_t;
					g2->n=o;
					g2->l.push_back(g1);
					g2->l.push_back(gt);
					g1=g2;
					continue;
				}
				s-=o.length();break;
			}
			g=g1;
			return 1;
		}
		s=t;return 0;
	}
	int okA/*=*/(cptr &s,gptr &g) //right associate version
	{
		cptr t=s;
		gptr gt,g1,g2;
		string o;
		if(okE(s,gt))
		{
			gptr g1=gt;
			if(rd(s,"=",o))
			{
				if(okA(s,gt))
				{
					g2=new grammar_t;
					g2->n=o;
					g2->l.push_back(g1);
					g2->l.push_back(gt);
					g=g2;
					return 1;
				}
			}
			else
			{
				g=gt;
				return 1;
			}
		}
		s=t;return 0;
	}
	int okE/*== !=*/(cptr &s,gptr &g)
	{
		cptr t=s;
		gptr gt,g1,g2;
		string o;
		if(okF(s,gt))
		{
			gptr g1=gt;
			while(rd(s,"==",o)||rd(s,"!=",o))
			{
				if(okF(s,gt))
				{
					g2=new grammar_t;
					g2->n=o;
					g2->l.push_back(g1);
					g2->l.push_back(gt);
					g1=g2;
					continue;
				}
				s-=o.length();break;
			}
			g=g1;
			return 1;
		}
		s=t;return 0;
	}
	int okF/*<><=>=*/(cptr &s,gptr &g)
	{
		cptr t=s;
		gptr gt,g1,g2;
		string o;
		if(okH(s,gt))
		{
			gptr g1=gt;
			while(rd(s,"<=",o)||rd(s,">=",o)||rd(s,"<",o)||rd(s,">",o))
			{
				if(okH(s,gt))
				{
					g2=new grammar_t;
					g2->n=o;
					g2->l.push_back(g1);
					g2->l.push_back(gt);
					g1=g2;
					continue;
				}
				s-=o.length();break;
			}
			g=g1;
			return 1;
		}
		s=t;return 0;
	}
	int okH/*+-*/(cptr &s,gptr &g)
	{
		cptr t=s;
		gptr gt,g1,g2;
		string o;
		if(okI(s,gt))
		{
			gptr g1=gt;
			while(rd(s,"+",o)||rd(s,"-",o))
			{
				if(okI(s,gt))
				{
					g2=new grammar_t;
					g2->n=o;
					g2->l.push_back(g1);
					g2->l.push_back(gt);
					g1=g2;
					continue;
				}
				s-=o.length();break;
			}
			g=g1;
			return 1;
		}
		s=t;return 0;
	}
	int okI/** /%*/(cptr &s,gptr &g)
	{
		cptr t=s;
		gptr gt,g1,g2;
		string o;
		if(okJ(s,gt))
		{
			gptr g1=gt;
			while(rd(s,"*",o)||rd(s,"/",o)||rd(s,"%",o))
			{
				if(okJ(s,gt))
				{
					g2=new grammar_t;
					g2->n=o;
					g2->l.push_back(g1);
					g2->l.push_back(gt);
					g1=g2;
					continue;
				}
				s-=o.length();break;
			}
			g=g1;
			return 1;
		}
		s=t;return 0;
	}
	int okJ/*!&*-*/(cptr &s,gptr &g)
	{
		gptr gt,g1,g2;
		cptr t=s;
		string o;
		if(rd(s,"!",o)||rd(s,"&",o)||rd(s,"*",o)||rd(s,"-",o))
			if(okL(s,gt))
			{
				g=new grammar_t;
				g->n=o;
				if(o=="-")
				{
					g1=new grammar_t;
					g1->n="0";
					g->l.push_back(g1);
				}
				g->l.push_back(gt);
				return 1;
			}
		s=t;
		return okL(s,g);
	}
	int okL_notused/*[]*/(cptr &s,gptr &g) //old
	{
		gptr gt,gt2,g1,g2;
		cptr t=s;
		string o;
		if(okZ(s,gt))
		{
			if(rd(s,'['))
				if(okS(s,gt2))
					if(rd(s,']'))
					{
						g=new grammar_t;
						g->n="[]";
						g->l.push_back(gt);
						g->l.push_back(gt2);
						return 1;
					}
		}
		s=t;
		return okZ(s,g);
	}
	int okL/*[]*/(cptr &s,gptr &g) //support multi array
	{
		gptr gt,gt2,g1,g2;
		cptr t=s;
		string o;
		if(okZ(s,gt))
		{
			gptr g1=gt;
			while(rd(s,"[",o))
			{
				cptr t2=s;
				if(okS(s,gt)&&rd(s,']'))
				{
					g2=new grammar_t;
					g2->n="[]";
					g2->l.push_back(g1);
					g2->l.push_back(gt);
					g1=g2;
				}
				else
				{
					s=t2;//recover okS + "]"
					s-=o.length();//recover "["
					break;
				}
			}
			g=g1;
			return 1;
		}
		s=t;
		return 0;
	}
	int okZ(cptr &s,gptr &g)//terminate elements, or expression inside parenthesis
	{
		g=new grammar_t;
		cptr t=s;
		gptr gt;
		if(rd(s,'@')) {g->n=token[s-_s0-1].s;return 1;}  //number
		if(rd(s,'$')) {g->n=token[s-_s0-1].s;return 1;}  //char const
		if(rd(s,'('))   //expression inside operator
			if(okS(s,gt))
				if(rd(s,')'))
				{
					g=gt;
					return 1;
				}
		s=t;
		if(okft(s,gt)) {g=gt;return 1;} //function call
		if(rd(s,'w')) {g->n=token[s-_s0-1].s;return 1;}  //variable name
		s=t;
		if(okZZ(s,gt)) {g=gt;return 1;} //array constant
		s=t;return 0;
	}
	int okZZ(cptr &s,gptr &g)//array constant
	{
		cptr t=s;
		gptr g1;
		g=new grammar_t;
		g->n="array_constant";
		if(rd(s,'{'))
			if(rd(s,'@'))
			{
				g1=new grammar_t;
				g1->n=token[s-_s0-1].s;
				g->l.push_back(g1);
				while(rd(s,",@"))
				{
					g1=new grammar_t;
					g1->n=token[s-_s0-1].s;
					g->l.push_back(g1);
				}
				if(rd(s,'}'))
					return 1;
			}
		s=t;return 0;
	}
	int prt(gptr g,int n=0)
	{
		int i,j;
		for(j=0;j<n;j++) printf("  ");
		printf("%s\n",g->n.c_str());
		for(i=0;i<g->l.size();i++)
		{
			prt(g->l[i],n+1);
		}
		return 0;
	}
	int run(char *s,const vector <token_t> &token_in,gptr &gm)
	{
		token=token_in;
		_s0=s;
		int sign=1;
		if(okproc(s,gm)&&(*s)==0)
			printf("Accepted by grammar analysis\n");
		else
		{
			sign=0;
			printf("Not accepted by grammar analysis\n");
			exit(-1);
		}
		if(_out)prt(gm);
		return sign;
	}
};

int syntax_analyze(char *s,const vector <token_t> &token_in,gptr &gm, map<string,string> &asm_mp)
{
	struct syntax_analyzer_t syntax_analyzer;
	syntax_analyzer.run(s,token_in,gm);
	asm_mp=syntax_analyzer.asm_mp;
	return 0;
}
