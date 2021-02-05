#include <io.h>
int main()
{
  char *a;int *b;char c;int i;
  a="hello world";
  b={0,1,2,3,4,5,6,7};
  c='z';
  puts(a);
  putc(10);
  for(i=0;i<8;i=i+1)
   {
     put32(b[i]);
     putc(' ');
   }
  putc(10);
  putc(c);
  putc(10);
  
}
