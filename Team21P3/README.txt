/*******************************************************************************
* File name: README.txt                                                        *
* Project: CS 460 Project 3 Fall 2019                                          *
* Author(s): Elizabeth Axworthy, Alexandria Labossiere, Roberto Rojas          *
* Date: 12/13/19                                                               *
* Description: This file contains README info                                  *
*******************************************************************************/

Instructions:
	1. make P3.out
	2. ./P3.out <filename>.ss
	(Optional to run cpp file)
	3. g++ <filename>.cpp Object.cpp
	4. ./a.out

This file takes a .ss file and translates it into a .cpp file. Also output are
a few other files (lst, dbg, p1, p2) which show the chain of rules the lexical
analyzer followed in seeing if the .ss file was correctly written, the lexemes
as they were processed, and other information useful for debugging.

I made a minor change to CodeGenerator.cpp:

void CodeGen::WriteCode (int tabs, string code)
{
	if(code.compare(";\n") != 0){
		for (int t = 0; t < tabs; t++)
			cpp << '\t';
		cpp << code;
	}
}

adding the outer if statement. This just makes the .cpp file cleaner, but
everything should work fine with the normal version too.
