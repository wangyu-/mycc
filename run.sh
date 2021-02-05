#! /bin/sh 
asm_format=`head -n1 output.asm |sed 's/;;format: //'`
if [ "$asm_format" = "bin" ]; then
	nasm -f bin output.asm -o output.exe
	dosbox -c "mount c ." -c "c:" -c "output.exe"
elif [ "$asm_format" = "elf32" ]; then
	nasm -f elf32 output.asm
	ld -m elf_i386 output.o -o output
	./output
elif [ "$asm_format" = "win32" ]; then
	nasm -f win32 output.asm
	ld -m i386pe output.obj kernel32.dll -o output32.exe     #to run this you need a copy of your kernel32.dll, typically from C:\Windows\SysWOW64\kernel32.dll
	#i686-w64-mingw32-gcc output.obj -o output32.exe
	#./GoLink.exe /console output.obj kernel32.dll
	wine output32.exe
	stty sane
else 
	echo "error: unknown format $asm_format"
fi;
