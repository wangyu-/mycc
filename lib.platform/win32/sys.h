int exit(int a)
{
__asm
{
	extern  _ExitProcess@4
	mov  eax,[a@exit]
	push eax
	call    _ExitProcess@4
}


}

