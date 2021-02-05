//nquee.c
/*
finds a solution for the n queen puzzle
*/
#include <io.h>
int board0[10000];
int *board[100];
int n;
int out_board(int r,int c)
{
	if(r>=n||c>=n||r<0||c<0) return 1;
	return 0;
}
int okay(int r,int c)
{
    int i;
    for(i=1;;i=i+1)
    {
	if(out_board(r-i,c)) break;  //test break
	if(board[r-i][c]) return 0;   
    }
    for(i=1;;i=i+1)
    {
	if(out_board(r-i,c-i)) goto out_loop; //test goto
	if(board[r-i][c-i]) return 0;   
    }
out_loop:
    for(i=1;!out_board(r-i,c+i);i=i+1)
	if(board[r-i][c+i]) return 0;   
    return 1;
}
int dfs(int row)
{
    int i;
    if (row==n) 
        return 1;
    for (i=0;i<n;i=i+1) 
    {
        if (!okay(row,i)) continue; /*test continue*/
	board[row][i]=1;
	if(dfs(row+1)) return 1;
	board[row][i]=0;
    }
    return 0;
}
int prt()
{
    int i;int j;
    for(i=0;i<n;i=i+1)
    {
        for (j=0;j<n;j=j+1)
	{
		if(board[i][j]) puts(" Q");
		else puts(" .");
	}
	newline();
    }
}

int main()
{
    int i;
    n=8;  //n can be changed, n<100
    for (i=0;i<n*n;i=i+1)
        board0[i]=0;
    for (i=0; i<n;i=i+1)
	board[i]=&board0[n*i];
    if(dfs(0))
	    prt();
    else
	    puts("no solution");
    return 0;
}
