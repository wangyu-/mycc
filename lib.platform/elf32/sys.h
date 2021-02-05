int exit(int a)
{
	__asm
	{
		mov eax,1
		mov ebx,[a@exit]
		int 0x80
	}
}
