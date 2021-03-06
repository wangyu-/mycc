all:
	g++ mycc.cpp asmcode.cpp input.cpp intermd.cpp lex.cpp syntax.cpp -Wno-write-strings  -ggdb -o mycc
	#g++ mycc.cpp -Wno-write-strings -fsanitize=address -O2 -ggdb
