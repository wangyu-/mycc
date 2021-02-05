#include <io.h>
#include <sys.h>
int main()
{
	char i;
	puts("press a key");
	newline();
	for(;;)
	{
		i=getc();
		puts("got char:");
		put32(i);
		newline();
		if(i<10) exit(0);
	}
}
