#include <io.h>
int cal(int a)
{
if(a==1) return 1;
else return a*cal(a-1);
}
int main()
{
int i;int j;
for(i=1;i<=9;i=i+1)
{
	for(j=1;j<=9;j=j+1)
{
put32(i);putc('*');put32(j);putc('=');put32(i*j);putc(' ');
}
newline();
}
}
