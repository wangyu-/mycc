#include <io.h>
int main()
{
int i;
for(i=0;i<254;)
{
__asm
  {
  mov ecx,[i@main]
  mov [a@putc],ecx
  inc ecx
  mov [i@main],ecx
  call putc
  }
}
}
