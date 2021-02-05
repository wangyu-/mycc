#include <io.h>
int main()
{
  int i;
  int j;
  for(j=0;j<999999;j=j+1)
{
  puts("input a number:");
  i=get32();
  puts("the number is:");
  put32(i);
  putc(10);
  puts("binary form:");
  putb(i);
  putc(10);
  putc(10);
}
}
