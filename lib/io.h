#include <_io.h>
#include <op.h>
int newline()
{
	putc(10);
}
int putb(int a)
{
   int i;
   int tmp;
   int arr[40];
   if(a<0) 
   {
   arr[0]='1';
   /*
   __asm
    {
    mov eax,[a@putb]
    and eax,7fffffffh
    mov [a@putb],eax   
    }*/
   a=and(a,2147483647);  //and with 7fffffffh
   }
   else arr[0]='0';
   for(i=0;i<31;i=i+1)
   {
     tmp=a%2;
     a=a/2;
     arr[31-i]=tmp+48;
   } 
   for(i=0;i<32;i=i+1)
     putc(arr[i]);
}

int put32(int a)
{
int b;int c;int d;
if(a==0) {putc('0');return 0;}
d=a;
c=0;
if(a<0) {putc('-');a=0-a;d=0-d;}
for(;;)
{
if(c==0) c=1;
else c=c*10;
a=a/10;
if(a==0) break;
}
a=d;
for(;;)
{
   if(c==0) break;
   putc(a/c+48);
   a=a-(a/c)*c;
   c=c/10;

}
return 0;
}

int get32()
{
  int a[100];
  int i;int sign;int rr;
rr=0;  
i=0;
  gets(a);
  if(a[0]==0) return 0;
  sign=1;
  if(a[i]=='+') {sign=1;i=i+1;}
  else if(a[i]=='-') {sign=0-1;i=i+1;}
  for(;a[i]>='0'&&a[i]<='9';i=i+1)
  {
    rr=rr*10;
    rr=rr+(a[i]-'0');   
  }
  return sign*rr;
}
int puts(char * a)
{
int i;
for(i=0;a[i]!=0;i=i+1)
{
putc(a[i]);
}
}
int gets(char *a)
{
int i;
for(i=0;;i=i+1)
{
a[i]=getc();
if(a[i]==10/*||a[i]==13*/) {a[i]=0;break;}
}
}
/*
int new(int a)
{
   __asm
   {
     mov eax,a@new
     mov  bx,ax
     mov ah,48h
     int 21h
     jnc line@a;
   }
   puts("error");
   __asm
  {
    line@a:
  }
}*/
