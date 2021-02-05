#include <io.h>
int cal(int a)
{
  if(a==1) return 1;
  else return cal(a-1)*a;
}
int main()
{
    put32(  cal(5)  );
}
