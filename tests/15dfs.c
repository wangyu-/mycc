#include <io.h>
int dfs(int n)
{
	int a[10];int i;
	if(n==10) return 0;
	for(i=0;i<10;i=i+1)
	{
		a[i]=n;
	}
	dfs(n+1);
	for(i=0;i<10;i=i+1)
	{
		put32(a[i]);
		putc(' ');
	}
	newline();
	return 0;
}
int dfs2(int n)
{
	int a;int b;int c;int d;int r;
	if(n==4) return 1;
	//a=n;b=n;c=n;d=n;
	a=b=c=d=n;
	r=(a+b)*dfs2(n+1)*(c+d);
	put32(a+b);
	putc(' ');
	put32(c+d);
	putc(' ');
	put32(r);
	newline();
	return r;
}
int main()
{
	int *x;
	x={1,2,3};
	dfs(0);
	dfs2(0);
	return 0;
}
