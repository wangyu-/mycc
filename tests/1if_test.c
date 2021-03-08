#include <io.h>
int max(int a,int b)
{
  if(a>b) return a;
  else return b;
}
int k;
int main()
{
   int i;int j;
   i=-2;j=-3;
   k=max(i,j);
   put32(k);
}
