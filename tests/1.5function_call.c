#include <io.h>
int max(int a,int b)
{
  if(a>b) return a;
	else return b;
}
int main()
{
  put32(max(2,3));
}
