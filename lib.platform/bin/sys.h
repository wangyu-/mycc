int exit(int a)
{
	__asm
	{
		mov ah,0x4c
		int 21h
	}
}
