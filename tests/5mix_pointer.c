#include <io.h>
int ***cal(int ******a)
{
int *b;
b=&a;
if(*b==1) return 1;
else return *b*cal(*b-1);
}
int main()
{
int i;int j;
int *a;int *b;
a=&i;b=&j;
for(*a=1;*a<=9;*a=*a+1)
{
	for(*b=1;*b<=9;*b=*b+1)
{
put32(*a);putc('*');put32(*b);putc('!');putc('=');put32(cal(*a**b));putc(' ');
}
newline();
}
}
