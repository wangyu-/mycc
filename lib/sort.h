int swap(int *a,int *b)
{
  int t;
  t=*a;*a=*b;*b=t;
}
int sort_partition(int* a,int p,int rr)
{
    int i;
    int j;int x;
    int t;
    i=p;j=rr+1;
    x=a[p];
    for(;;)
    {
       i=i+1;
       for(;a[i]<x&&i<rr;) i=i+1;
       j=j-1; 
       for(;a[j]>x;) j=j-1;
       if(i>=j) break;  
       t=a[i];a[i]=a[j];a[j]=t;
    }
    a[p]=a[j];
    a[j]=x;
    return j;

}
int sort(int *a,int p,int r)
{
  __sort(a,p,r-1);
}
int __sort(int * a,int p,int rr)
{
  int _q;
  if(p<rr)
  {
  _q=sort_partition(a,p,rr);
  __sort(a,p,_q-1);
  __sort(a,_q+1,rr);
  }
  return 0;
}
int sortn(int * a,int p,int rr)
{
  int i;int t;int c;int d;
  rr=rr-1;
  __sort(a,p,rr);
  for(i=p;i<rr-i;i=i+1)
  {
      swap(&(a[i]),&(a[rr-i]));
      //t=a[i];a[i]=a[rr-i];a[rr-i]=t;     
  }
}
