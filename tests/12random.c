#include <io.h>
#include <rand.h>
#include <op.h>
#include <sort.h>
int main()
{
  int arr[10005];
  int i;
  srand(0);
  for(i=0;i<10000;i=i+1)
   {
	arr[i]=rand();
   }
  sort(arr,0,10000);
  for(i=0;i<10000;i=i+1)
   {
	put32(arr[i]);
	newline();
   }
  
}
