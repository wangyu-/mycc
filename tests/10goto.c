#include <io.h>
int main()
{
   puts("1")  ;
   goto line1;
   puts("2") ;
   line1:;
   puts("3") ;
}
