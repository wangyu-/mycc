#include <io.h>
int main()
{
int i;
int a[100];
for(i=0;i<100;i=i+1)
  a[i]=i;
for(i=0;i<100;i=i+1)
  {
   put32(a[i]);
   putc(' ');
  }
}
