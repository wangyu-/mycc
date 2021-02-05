#include <io.h>
#include <rand.h>
#include <op.h>
#include <sort.h>
int rec(int depth)
{
	if(depth%100==0)
	{
		puts("depth=");
		put32(depth);
		newline();
	}
	rec(depth+1);
}
int main()
{
	rec(0);
}
