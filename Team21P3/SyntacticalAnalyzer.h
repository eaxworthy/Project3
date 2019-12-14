/*******************************************************************************
* File name:                                                                   *
* Project: CS 460 Project 2 Fall 2019                                          *
* Author(s):                                                                   *
* Date:                                                                        *
* Description: This file contains                                              *
*******************************************************************************/

#ifndef SYN_H
#define SYN_H

#include <iostream>
#include <fstream>
#include "CodeGenerator.h"
#include "LexicalAnalyzer.h"

using namespace std;

class SyntacticalAnalyzer
{
    public:
	SyntacticalAnalyzer (char * filename);
	~SyntacticalAnalyzer ();
    private:
	LexicalAnalyzer * lex;
  CodeGen * cg;
	token_type token;
	ofstream debug2;
	ofstream P2;
  string code;
  int tabs;
  int params;
  string op;
  string returnstmt;
	int program ();
	int more_defines ();
	int define ();
	int stmt_list ();
	int stmt ();
	int literal ();
	int quoted_lit ();
	int more_tokens ();
	int param_list ();
	int else_part ();
	int stmt_pair ();
	int stmt_pair_body ();
	int action ();
	int any_other_token ();
};

#endif
