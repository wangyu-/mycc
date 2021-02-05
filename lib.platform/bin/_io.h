#include <sys.h>
int putc(char a)
{
__asm
{
mov eax,[a@putc]
mov dl,al
MOV AH,02H
INT 21H
}

}

char getc()
{
  char a;
  __asm
  {
     xor eax,eax
     mov ah,01h
     int 21h
     xor ah,ah
     mov [a@getc],eax
  }
  if(a==13) return 10;  // in dos "enter" produces \r (no \n)
  if(a==3) exit(255);   //dos doesn't recongize ctrl-c, we need a way to quit
  return a;
}
