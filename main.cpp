#include <iostream>
#include "x86Parser.h"




int main(int argc,  char* argv[])
{
    x86Parser p;
    p.parseFile(argv[1]); 
    
    return 0;
}