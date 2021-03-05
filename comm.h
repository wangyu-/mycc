#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <stack>
using namespace std;
const int _out=1;

enum asm_format_t{format_bin=0,format_elf32,format_win32};
const char format_name_table[][100]={"bin","elf32","win32"};

extern asm_format_t g_asm_format;
extern string g_format_name;
