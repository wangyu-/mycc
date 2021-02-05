int and(int a,int b)
{
	int r;
	__asm
	{
		mov eax,[a@and]
		and eax,[b@and]
		mov [r@and],eax
	}
	return r;
}

int xor(int a,int b)
{
	int r;
	__asm
	{
		mov eax,[a@and]
		xor eax,[b@and]
		mov [r@and],eax
	}
	return r;
}

int or(int a,int b)
{
	int r;
	__asm
	{
		mov eax,[a@and]
		or eax,[b@and]
		mov [r@and],eax
	}
	return r;
}
