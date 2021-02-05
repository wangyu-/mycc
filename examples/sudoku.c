//sudoku.c
/*
fills the sudoku game
*/
#include <io.h>

int num[10];
int pnt_x[100];
int pnt_y[100];
int pcnt;

char mat0[81];
char * mat[9];

int pretty_print;

int zero(int *a,int n)
{
	int i;
	for(i=0;;i=i+1)
	{
		if(i==n) break; //test break
		a[i]=0;
	}
}
int okay(int x,int y)
{ 
	int i;int j;int k;int u;int v;
	i=x;
	zero(num,10);
	for(j=0;j<9;j=j+1)
	{
		if(!mat[i][j]) continue;  //test continue
		if(num[ mat[i][j] ]) return 0;
		num[ mat[i][j] ]=1;
	}
	i=y;
	zero(num,10);
	for(j=0;j<9;j=j+1)
	{
		if(mat[j][i])
		{
			if(num[ mat[j][i] ]) goto end_func; /* test goto */
			num[ mat[j][i] ]=1;
		}
	}
	i=x/3;
	j=y/3;
	zero(num,10);
	for(u=0;u<3;u=u+1)
		for(v=0;v<3;v=v+1)
		{
			if(mat[i*3+u][j*3+v])
			{
				if(num[  mat[i*3+u][j*3+v]  ]) return 0;
				num[  mat[i*3+u][j*3+v]  ]=1;
			}
		}
	return 1;
end_func:
	return 0;
}
int dfs(int s)
{
	int i;
	if(s==pcnt) return 1;
	for(i=1;i<=9;i=i+1)
	{
		mat[pnt_x[s]][pnt_y[s]]=i;
		if(okay(pnt_x[s],pnt_y[s])) 
		{
			if(dfs(s+1)==1) return 1;
		}
		mat[pnt_x[s]][pnt_y[s]]=0;
	}
	return 0;
}
int prt()
{
	int i;int j;
	for(i=0;i<9;i=i+1)
	{
		if(pretty_print)
			if(i==3||i==6) {puts("-------+-------+------");newline();}
		for(j=0;j<9;j=j+1)
		{
			if(pretty_print)
			{
				if(j==3||j==6) puts(" |");
				putc(' ');
			}
			if(mat[i][j])
				put32(mat[i][j]);
			else(putc('.'));
		}
		newline();
	}
}
int main()
{

	int i;int j;
	pretty_print=1;
	pcnt=0;
	for(i=0;i<9;i=i+1)
		mat[i]=&mat0[i*9];
	for(i=0;i<9;i=i+1)
		gets(mat[i]);

	for(i=8;i>=0;i=i-1)
		for(j=8;j>=0;j=j-1)
		{
			
			if(mat[i][j]>='1'&&mat[i][j]<='9')
			{
				mat[i][j]=mat[i][j]-48;
			}
			else
			{
				mat[i][j]=0;
				pnt_x[pcnt]=i;
				pnt_y[pcnt]=j;
				pcnt=pcnt+1;
			}
		}
	puts("input:");newline();
	prt();
	newline();
	if (dfs(0)==0) {puts("no solution");newline();}
	else
	{

		puts("output:");newline();
		prt();
	}

	return 0;
}
