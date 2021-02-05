int putc(char a)
{
__asm
{
mov eax,4            
mov ebx,1
mov ecx,[a@putc]
push ecx
mov ecx,esp
mov edx,1
int 80h
add   esp, 4
}

}

char getc()
{
  char a;
  a=0;
  __asm
  {
	mov eax, 3   ;the syscall num of "read()" on linux
	mov ebx, 0   ;read from stdin, 0 is the fd of stdin
	mov ecx, a@getc   ;save the result to the address of variable "a" inside function "getc"
	mov edx, 1   ;read 1 character
	int 0x80
  }
  return a;
}
