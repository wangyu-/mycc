#include <io.h>
int main()
{
 int a;int b;int c;int d;
a=&c;
b=&d;
*a=3;
*b=4;
put32(*a);
put32(*b);
put32(c);
put32(d);
}
