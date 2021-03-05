all:
	g++ mycc.cpp comm.cpp -Wno-write-strings  -ggdb -o mycc
	#g++ mycc.cpp -Wno-write-strings -fsanitize=address -O2 -ggdb
