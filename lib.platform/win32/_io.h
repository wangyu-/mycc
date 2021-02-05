int putc(char a)
{
/*
__asm
{
extern _putchar
push eax
call _putchar
add  esp, 4
}*/
int buffer2[2];
__asm
{
extern _GetStdHandle@4
extern _WriteConsoleA@20
push -11
call _GetStdHandle@4
push    0
push    arr?buffer2@putc
push    1
push    a@putc
push    eax
call    _WriteConsoleA@20
}


}

char getc()
{
/*
  int a;
  __asm
  {
extern _getchar
call _getchar
mov [a@getc],eax
  }*/
char a;
int buffer2[2];
a=0;
__asm
{
extern _GetStdHandle@4
extern _ReadConsoleA@20
push -10
call _GetStdHandle@4

push 0
push arr?buffer2@getc
push 1
push a@getc
push eax
call _ReadConsoleA@20
}
  if(a==13) {getc();return 10;}   //on windows "enter" produces \r\n .. 
  return a;
}
