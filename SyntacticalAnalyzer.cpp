/*******************************************************************************
* File name: SyntacticalAnalyzer.cpp                                           *
* Project: CS 460 Project 2 Fall 2019                                          *
* Author(s): Haley Psomas-Sheridan, Gleb Krunchinin, Alexandria Labossiere     *
*            [Modifications by Elizabeth Axworthy, Alexandria Labossiere]      *
*             and Roberto Rojas for Project 3                                  *
* Date: 12/12/19                                                               *
* Description: This file contains code for all the functions provided          *
* in  SyntacticalAnalyzer.h. Outputs rules to a file named                     *
* <input filename>.p2  outputs all error messages to                           *
* <input filename>.debug and <input filename>.lst                              *
*******************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include "SyntacticalAnalyzer.h"
#include <map>
#include <set>

using namespace std;


SyntacticalAnalyzer::SyntacticalAnalyzer (char * filename)
{
  lex = new LexicalAnalyzer (filename);
  string nameOfFile = filename;

  cg = new CodeGen (nameOfFile);
  token_type t;

  //set up output p2 file
  string p2name = nameOfFile.substr (0, nameOfFile.length()-3) + ".p2";
  P2.open(p2name);

  //set up output debug file
  string debug2name = nameOfFile.substr (0, nameOfFile.length()-3) + ".dbg";
  debug2.open(debug2name);
  code = "";
  op = "";
  returnstmt = "";
  tabs = 0;
  params = 0;
  int totalErrors = program ();
}


SyntacticalAnalyzer::~SyntacticalAnalyzer (){
  debug2.close();
  P2.close();
  delete lex;
  delete cg;
}


//   1    <program> -> LPAREN_T <define> LPAREN_T <more_defines> EOF_T
int SyntacticalAnalyzer::program (){

  int errors = 0;
  //if you use an array, you'll need to do the check manually
  set<token_type> first = {LPAREN_T, COND_T};
  set<token_type> follows = {EOF_T}; //if empty, you can put EOF_T
  token = lex->GetToken();

  if(token == NONE){
    debug2 << "Error retrieving token" << endl;
    return 1;
  }

  P2 << "Using Rule 1" << endl;

  if(token == LPAREN_T){
    token = lex->GetToken();
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;

  }
  else{
    errors ++;

    debug2 << "Error in program: expecting LPARENT_T, got: " << lex->GetTokenName(token) << endl;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
  }

  errors += define();

  if(token == LPAREN_T){
    token = lex->GetToken();
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
  }
  else{
    errors ++;
    debug2 << "Error in program: expecting LPARENT_T, got: " << lex->GetTokenName(token) << endl;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
  }

  errors += more_defines();

  if(token != EOF_T){
    errors ++;
    debug2 << "Error in program: expecting EOF_T, got: " << lex->GetTokenName(token) << endl;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
  }

  return errors;
}


//2    <more_defines> -> <define> LPAREN_T <more_defines>
//3    <more_defines> -> IDENT_T <stmt_list> RPAREN_T
int SyntacticalAnalyzer::more_defines (){

  int errors = 0;

  set<token_type> first = {IDENT_T, DEFINE_T};
  set<token_type> follows = {EOF_T}; //if empty, you can put EOF_T

  while(first.find(token) == first.end()){

    if(token == EOF_T){
      debug2 << "Unexpected EOF_T" << endl;
      return errors+1;
    }
    debug2 << "Error: in firsts of more_defines" << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    token = lex->GetToken();
  }

  if(token == DEFINE_T){
    P2 << "using Rule 2" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;

    errors += define();

    if(token == LPAREN_T){
      debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
      token = lex-> GetToken();
      errors += more_defines();
    }
    else{
      errors++;
      debug2 << "Error in more_defines: expecting LPARENT_T, got: " << lex->GetTokenName(token) << endl;
      lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    }
  }
  else if (token == IDENT_T){
    P2 << "using Rule 3" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex -> GetToken();

    errors +=stmt_list();

    if(token == RPAREN_T){
      debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
      token = lex -> GetToken();
    }
    else{
      errors++;
      debug2 << "Error in more_defines: expecting RPARENT_T, got: " << lex->GetTokenName(token) << endl;
      lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    }
  }
  else{
    errors ++;
    debug2 << "Error in more_defines: expecting IDENT_T, or DEFINE_T, got: " << lex->GetTokenName(token) << endl;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
  }


  return errors;
}


//4  <define> -> DEFINE_T LPAREN_T IDENT_T <param_list> RPAREN_T <stmt> <stmt_list> RPAREN_T
int SyntacticalAnalyzer::define (){

  P2 << "Using Rule 4" << endl;
  int errors = 0;

  if(token == DEFINE_T){
    debug2 << lex->GetTokenName(token) <<" " << lex -> GetLexeme()<< endl;
    token = lex->GetToken();
  }
  else{
    debug2 << "Error in define_t: expecting DEFINE_T, got: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
  }

  if(token == LPAREN_T){
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
  }
  else{
    debug2 << "Error in define_t: expecting LPARENT_T, got: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected " + lex->GetLexeme());
  }

  if(token == IDENT_T){
    if(lex->GetLexeme().compare("main") == 0){
      code += "int main(";
      returnstmt = "\treturn 0;\n";
    }
    else{
      code += "Object " + lex->GetLexeme() + "(";
      returnstmt = "\treturn __RetVal;\n";
    }
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
  }
  else{
    debug2 << "Error in define_t: expecting IDENT_T, got: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");

  }

  errors += param_list();
  params = 0;
  cg->WriteCode(tabs, code);
  code = "{\n\tObject __RetVal;\n";
  cg->WriteCode(tabs++, code);
  code = "";
  if(token == RPAREN_T){
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
  }
  else{
    debug2 << "Error in define_t: expecting RPARENT_T, got: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
  }

  errors += stmt();
  errors +=stmt_list();

  if(token == RPAREN_T){
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();
  }
  else{
    debug2 << "Error in define_t: expecting RPARENT_T, got: " << lex->GetTokenName(token) << endl;;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
  }
  cg->WriteCode(--tabs, returnstmt + "}\n");
  code = "";
  return errors;
}


//  5    <stmt_list> -> <stmt> <stmt_list>
//  6    <stmt_list> -> {}
int SyntacticalAnalyzer::stmt_list (){

  set<token_type> first = {IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T, RPAREN_T};
  set<token_type> follows = { RPAREN_T };

  int errors = 0;

  while(first.find(token) == first.end()){

    if(token == EOF_T){
      debug2 << "Unexpected EOF_T" << endl;
      return errors+1;
    }
    debug2 << "Error in stmt_list! unexpected: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    token = lex->GetToken();
  }

  if(token == RPAREN_T){
    P2 << "Using Rule 6" << endl;
    return errors;
  }
  else if(first.find(token) != first.end()){
    P2 << "Using Rule 5" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;

    errors += stmt();
    if(op.compare("") != 0){
      code += op;
      op = "";
    }
    errors += stmt_list();

  }
  else{
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    debug2 << "Error in stmt_list: expecting RPARENT_T or something in stmt got: " << lex->GetTokenName(token) << endl;
  }
  return errors;
}


// We enter stmt with some variable prewritten to code that we will be adding to
// so don't have '\n' and the beginning of the string you add to code.
int SyntacticalAnalyzer::stmt (){

  int errors = 0;

  set<token_type> first = {IDENT_T, LPAREN_T,  SQUOTE_T, NUMLIT_T, STRLIT_T};
  set<token_type> follows = { RPAREN_T, IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T};

  while(first.find(token) == first.end()){

    if(token == EOF_T || follows.find(token) != follows.end()){
      lex-> ReportError("unexpected " + lex->GetLexeme());
      return errors+1;
    }

    debug2 << "Error in stmt! unexpected: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    token = lex->GetToken();
  }

  if(token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T){ //7  <stmt> -> <literal>
    P2 << "Using Rule 7" << endl;
    debug2 << lex->GetTokenName(token) <<" " << lex -> GetLexeme()<< endl;
    errors += literal();
  }
  else if(token == IDENT_T){
    P2 << "Using Rule 8" << endl;
    code += lex->GetLexeme() + " ";
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
  }
  else if(token == LPAREN_T){
    P2 << "Using Rule 9" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    errors += action();
    if(token = RPAREN_T){
      token = lex->GetToken();
      debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    }
    else{
      lex-> ReportError("unexpected " + lex->GetLexeme());
      debug2 << "Error in stmt: expecting RPARENT_T, got: " << lex->GetTokenName(token) << endl;
    }
  }
  else{
    errors ++;
    lex-> ReportError("unexpected " + lex->GetLexeme());
    debug2 << "Error in stmt: expecting LPARENT_T or IDENT_T, got: " << lex->GetTokenName(token) << endl;
  }
  //cg->WriteCode(tabs, code);
  //code = "";
  return errors;
}


// 10    <literal> -> NUMLIT_T
// 11    <literal> -> STRLIT_T
// 12    <literal> -> SQUOTE_T <quoted_lit>
int SyntacticalAnalyzer::literal (){

  int errors = 0;
  set<token_type> first = {NUMLIT_T, STRLIT_T, SQUOTE_T};
  set<token_type> follows = {RPAREN_T, IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T };

  while(first.find(token) == first.end()){
    if(token == EOF_T){
      debug2 << "Unexpected EOF_T" << endl;
      return errors+1;
    }
    debug2 << "Error! unexpected: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    token = lex->GetToken();
  }

  if(token == NUMLIT_T){
    P2 << "Using Rule 10" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    code += "Object(" + lex->GetLexeme() + ")";
    token = lex->GetToken();
  }

  else if(token == STRLIT_T){ //11  <literal> -> STRLIT_T
    P2 << "Using Rule 11" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    code += "Object(" + lex->GetLexeme() + ")";
    token = lex->GetToken();
  }

  else if( token == SQUOTE_T){ //12  <literal> -> SQUOTE_T <quoted_lit>
    P2 << "Using Rule 12" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    code += "Object(\"";
    errors+= quoted_lit();
    code.erase(code.end()-2);
    code += "\")";
  }
  else{
    errors ++;
    debug2 << "Error in literal: expecting NUMLIT_T or STRLIT_T or SQUOTE_T, got: " << lex->GetTokenName(token) << endl;
  }


  return errors;
}

// 13  <quoted_lit> -> <any_other_token>
int SyntacticalAnalyzer::quoted_lit (){

  int errors = 0;

  set<token_type> first = { LISTOP1_T, LISTOP2_T, IF_T, COND_T, AND_T, OR_T, NOT_T, NUMBERP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T, PLUS_T, MINUS_T, DIV_T, MULT_T, MODULO_T, ROUND_T, EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T, IDENT_T, DISPLAY_T, NEWLINE_T, LPAREN_T, NUMLIT_T, STRLIT_T, DEFINE_T, SQUOTE_T, ELSE_T, RPAREN_T};
  set<token_type> follows = {RPAREN_T, IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T };

  while(first.find(token) == first.end()){

    if(token == EOF_T){
      debug2 << "Unexpected EOF_T " << endl;
      return errors+1;
    }
    debug2 << "Error in quoted_lit! unexpected: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    token = lex->GetToken();
  }

  //13    <quoted_lit> -> <any_other_token>
  P2 << "Using Rule 13" << endl;
  debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
  errors += any_other_token();


  return errors;
}


// 14    <more_tokens> -> <any_other_token> <more_tokens>
// 15    <more_tokens> -> {}
int SyntacticalAnalyzer::more_tokens (){

  int errors = 0;

  set<token_type> first = {LISTOP1_T, LISTOP2_T, IF_T, COND_T, AND_T, OR_T, NOT_T, NUMBERP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T, PLUS_T, MINUS_T, DIV_T, MULT_T, MODULO_T, ROUND_T, EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T, IDENT_T, DISPLAY_T, NEWLINE_T, LPAREN_T, NUMLIT_T, STRLIT_T, DEFINE_T, SQUOTE_T, ELSE_T, RPAREN_T};
  set<token_type> follows = {RPAREN_T };

  while(first.find(token) == first.end()){

    if(token == EOF_T){
      debug2 << "Unexpected EOF_T " << endl;
      return errors+1;
    }
    debug2 << "Error more_tokens! unexpected: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    token = lex->GetToken();
  }

  if(follows.find(token) == follows.end()){//if the token is not in the follows list
    //14 <more_tokens> -> <any_other_token> <more_tokens>
    P2 << "Using Rule 14" << endl;
    debug2  << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;

    errors += any_other_token();
    errors += more_tokens();
  }
  else if(token == RPAREN_T){ //15  <more_tokens> -> {}
  P2 << "Using Rule 15" << endl;
  debug2  << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
}
else{
  errors++;
  lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
  debug2 << "Error more_tokens: in more_tokens, got: " << lex->GetTokenName(token) << endl;
}

return errors;
}


// 16    <param_list> -> IDENT_T <param_list>
// 17    <param_list> -> {}
int SyntacticalAnalyzer::param_list (){

  int errors = 0;
  set<token_type> first = {IDENT_T, RPAREN_T };
  set<token_type> follows = { RPAREN_T};

  while(first.find(token) == first.end()){

    if(token == EOF_T){
      debug2 << "Unexpected EOF_T " << endl;
      return errors+1;
    }
    debug2 << "Error param_list! unexpected: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    token = lex->GetToken();
  }

  if(token == IDENT_T){//16  <param_list> -> IDENT_T <param_list>
    P2 << "Using Rule 16" << endl;
    if(params)
    code += ", Object " + lex->GetLexeme();
    else
    code += "Object " + lex->GetLexeme();
    debug2 << lex->GetTokenName(token) <<" " << lex -> GetLexeme()<< endl;
    token = lex->GetToken();
    params = 1;
    errors += param_list();

  }
  //17  <param_list> -> {}
  else if (token == RPAREN_T){
    params = 0;
    P2 << "Using Rule 17" << endl;
    code += ")";
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
  }
  else{
    debug2 << "Error in param_list: expecting RPARENT_T or IDENT_T, got: " << lex->GetTokenName(token) << endl;
    lex-> ReportError("unexpected " + lex->GetLexeme());
    errors ++;
  }

  return errors;
}


// 18    <else_part> -> <stmt>
// 19    <else_part> -> {}
int SyntacticalAnalyzer::else_part (){

  int errors = 0;
  set<token_type> first = {IDENT_T, LPAREN_T, RPAREN_T, NUMLIT_T, SQUOTE_T, STRLIT_T };
  set<token_type> follows = {RPAREN_T };

  while(first.find(token) == first.end()){

    if(token == EOF_T){
      debug2 << "Unexpected EOF_T " << endl;
      return errors+1;
    }
    debug2 << "Error in else_part! unexpected: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    token = lex->GetToken();
  }

  if(token == RPAREN_T){ //  19    <else_part> -> {}
  P2 << "Using Rule 19" << endl;
  debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;

}
else { //18    <else_part> -> <stmt>
  P2 << "Using Rule 18" << endl;
  debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
  errors += stmt();
}

return errors;
}


// 20    <stmt_pair> -> LPAREN_T <stmt_pair_body>
// 21    <stmt_pair> -> {}
int SyntacticalAnalyzer::stmt_pair (){

  int errors = 0;
  set<token_type> first = { LPAREN_T, RPAREN_T};
  //set<token_type> follows = { RPAREN_T};

  while(first.find(token) == first.end()){

    if(token == EOF_T){
      debug2 << "Unexpected EOF_T " << endl;
      return errors+1;
    }
    debug2 << "Error in stmt_pair! unexpected: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    token = lex->GetToken();
  }

  if(token == LPAREN_T){// 20    <stmt_pair> -> LPAREN_T <stmt_pair_body>
    P2 << "Using Rule 20" << endl;
    debug2 << lex->GetTokenName(token) << endl;
    token = lex->GetToken();
    errors += stmt_pair_body();
  }
  else if(token == RPAREN_T){// 21  <stmt_pair> -> {}
  P2  << "Using Rule 21" << endl;
  debug2 << lex->GetTokenName(token) << endl;
}
else{
  lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
  errors++;
}

return errors;
}


// 22    <stmt_pair_body> -> <stmt> <stmt> RPAREN_T <stmt_pair>
// 23    <stmt_pair_body> -> ELSE_T <stmt> RPAREN_T
int SyntacticalAnalyzer::stmt_pair_body (){

  int errors = 0;
  set<token_type> first = { ELSE_T, IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T};
  set<token_type> follows = { RPAREN_T};

  while(first.find(token) == first.end()){

    if(token == EOF_T){
      debug2 << "Unexpected EOF_T " << endl;
      return errors+1;
    }
    debug2 << "Error stmt_pair_body! unexpected: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    token = lex->GetToken();
  }

  if(token == ELSE_T){//  23    <stmt_pair_body> -> ELSE_T <stmt> RPAREN_T
    P2 << "Using Rule 23" << endl;
    debug2 << lex->GetTokenName(token) << endl;
    token = lex -> GetToken();

    errors += stmt();

    if(token == RPAREN_T){
      debug2 << lex->GetTokenName(token) << endl;
      token = lex -> GetToken();
    }
    else{
      debug2 << "Error in stmt_pair_body: expecting ELSE_T, got: " << lex->GetTokenName(token) << endl;
      errors++;
      lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    }

  }
  else{//  22    <stmt_pair_body> -> <stmt> <stmt> RPAREN_T <stmt_pair>
    P2 << "Using Rule 22" << endl;
    debug2 << lex->GetTokenName(token) << endl;

    errors+= stmt();
    errors+= stmt();

    if(token == RPAREN_T){
      token = lex -> GetToken();
      debug2 << lex->GetTokenName(token) << endl;
    }
    else {
      debug2 << "Error in stmt_pair_body: expecting RPARENT_T, got: " << lex->GetTokenName(token) << endl;
      errors++;
      lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    }

    errors += stmt_pair();
  }

  return errors;
}

int SyntacticalAnalyzer::action (){

  int errors = 0;

  set<token_type> first = { LISTOP1_T, LISTOP2_T, IF_T, COND_T, AND_T, OR_T, NOT_T, NUMBERP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T, PLUS_T, MINUS_T, DIV_T, MULT_T, MODULO_T, ROUND_T, EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T, IDENT_T, DISPLAY_T, NEWLINE_T};

  set<token_type> follows = { RPAREN_T};

  switch (token){

    //Written, needs testing
    case IF_T://24 <action> -> IF_T <stmt> <stmt> <else_part>
          P2 << "Using Rule 24" << endl;
          debug2 << lex->GetTokenName(token) << endl;
          token = lex->GetToken();
          code += "if(";
          errors += stmt();
          code+= "){\n";
          cg->WriteCode(tabs++, code);
          code = "";
          errors +=stmt();
          cg->WriteCode(tabs-1, "}\nelse{\n");
          code = "";
          errors += else_part();
          cg->WriteCode(--tabs, "}\n");
          code = "";
          break;

    case COND_T: //25<action> -> COND_T LPAREN_T <stmt_pair_body>
    P2 << "Using Rule 25" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();

    if(token == LPAREN_T){
      token = lex-> GetToken();
      errors+= stmt_pair_body();
    }
    else{
      errors++;
      lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
      debug2 << "Error! unexpected: " << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    }
    break;

    case LISTOP1_T://26  <action> -> LISTOP1_T <stmt>
    P2 << "Using Rule 26" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();
    errors+= stmt();
    break;

    case LISTOP2_T:// 27 <action> -> LISTOP2_T <stmt> <stmt>
    P2 << "Using Rule 27" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();

    errors = stmt();
    errors = stmt();
    break;

    case AND_T: //28    <action> -> AND_T <stmt_list>
          P2 << "Using Rule 28" << endl;
          debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
          token = lex-> GetToken();
          code += " && ";
          errors += stmt_list();
          break;

    case OR_T://29    <action> -> OR_T <stmt_list>
          P2 << "Using Rule 29" << endl;
          debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
          token = lex-> GetToken();
          code += " || ";
          errors +=stmt_list();
          break;

    case NOT_T: // 30    <action> -> NOT_T <stmt>
          P2 << "Using Rule 30" << endl;
          debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
          token = lex-> GetToken();
          code += " !(";
          errors += stmt();
          break;

    case NUMBERP_T:// 31    <action> -> NUMBERP_T <stmt>
    P2 << "Using Rule 31" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();

    errors += stmt();
    break;

    case LISTP_T://32    <action> -> LISTP_T <stmt>
    P2 << "Using Rule 32" << endl;
    debug2 << lex->GetTokenName(token) <<" " << lex -> GetLexeme()<< endl;
    token = lex-> GetToken();

    errors += stmt();
    break;

    case ZEROP_T: //33    <action> -> ZEROP_T <stmt>
    P2 << "Using Rule 33" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();

    errors += stmt();
    break;

    case NULLP_T://34    <action> -> NULLP_T <stmt>
    P2 << "Using Rule 34" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();

    errors +=stmt();
    break;

    case STRINGP_T://35    <action> -> STRINGP_T <stmt>
    P2 << "Using Rule 35" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();

    errors += stmt();
    break;

    //Written, needs testing
    case PLUS_T: // 36    <action> -> PLUS_T <stmt_list>
    P2 << "Using Rule 36" << endl;
    op = " + ";
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();
    errors += stmt_list();
    op = "";
    break;

    //Written, needs testing
    case MINUS_T: //37    <action> -> MINUS_T <stmt> <stmt_list>
    P2 << "Using Rule 37" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();

    errors+= stmt();
    code += " - ";
    errors += stmt_list();
    break;

    //Written, needs testing
    case DIV_T:// 38    <action> -> DIV_T <stmt> <stmt_list>
    P2 << "Using Rule 38" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();

    errors+= stmt();
    code += " \\ ";
    errors+=stmt_list();
    break;

    //Written, needs testing
    case MULT_T://39    <action> -> MULT_T <stmt_list>
    P2 << "Using Rule 39" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();
    op = " * ";
    errors += stmt_list();
          op = "";
    break;

    //Written, needs testing
    case MODULO_T:// 40    <action> -> MODULO_T <stmt> <stmt>
    P2 << "Using Rule 40" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();

    errors += stmt();
    code += " % ";
    errors += stmt();
    break;

    case ROUND_T:// 41    <action> -> ROUND_T <stmt>
    P2  << "Using Rule 41" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();

    errors += stmt();
    break;

    //Written, needs testing
    case EQUALTO_T: //42 <action> -> EQUALTO_T <stmt_list>
    P2 << "Using Rule 42" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();
    op = " == ";
    errors += stmt_list();
          op = "";
    break;

    //Written, needs testing
    case GT_T:// 43    <action> -> GT_T <stmt_list>
    P2 << "Using Rule 43" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();
    op = "> ";
    errors += stmt_list();
    op = "";
    break;

    case LT_T:// 44    <action> -> LT_T <stmt_list>
    P2 << "Using Rule 44" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();
          op = "< ";
    errors += stmt_list();
          op = "";
    break;

    case GTE_T:// 45    <action> -> GTE_T <stmt_list>
    P2 << "Using Rule 45" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();
          op = ">= ";
    errors += stmt_list();
          op = "";
    break;

    case LTE_T:// 46    <action> -> LTE_T <stmt_list>
    P2 << "Using Rule 46" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();
          op = "<= ";
    errors += stmt_list();
          op = "";
    break;

    case IDENT_T:// 47    <action> -> IDENT_T <stmt_list>
    P2  << "Using Rule 47" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    code += lex->GetLexeme() + '(';
    token = lex-> GetToken();
    errors += stmt_list();
    code += ");\n";
    cg->WriteCode(tabs, code);
    code = "";
    break;

    case DISPLAY_T:// 48    <action> -> DISPLAY_T <stmt>
    P2 << "Using Rule 48" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex-> GetToken();
    code += "cout << ";
    errors += stmt();
    code += ";\n";
    cg->WriteCode(tabs, code);
    code = "";
    break;

    case NEWLINE_T:
    P2 << "Using Rule 49" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    code = "cout << endl;\n";
    cg->WriteCode(tabs, code);
    code = "";
    token = lex-> GetToken();
    break;

    default:
    debug2 << "Error in action! unexpected: " << lex->GetTokenName(token) << endl;
    errors++;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    break;
  }
  return errors;
}

int SyntacticalAnalyzer::any_other_token (){

  int errors = 0;
  code += lex->GetLexeme()+ " ";
  set<token_type> first = { LISTOP1_T, LISTOP2_T, IF_T, COND_T,AND_T, OR_T, NOT_T, NUMBERP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T, PLUS_T, MINUS_T, DIV_T, MULT_T, MODULO_T, ROUND_T, EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T, IDENT_T, DISPLAY_T, NEWLINE_T, LPAREN_T, NUMLIT_T, STRLIT_T, DEFINE_T, SQUOTE_T, ELSE_T};


  set<token_type> follows = { LISTOP1_T, LISTOP2_T, IF_T, COND_T, AND_T, OR_T, NOT_T, NUMBERP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T, PLUS_T, MINUS_T, DIV_T, MULT_T, MODULO_T, ROUND_T, EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T, IDENT_T, DISPLAY_T, NEWLINE_T, LPAREN_T, NUMLIT_T, STRLIT_T, DEFINE_T, SQUOTE_T, ELSE_T, RPAREN_T, IDENT_T, LPAREN_T, NUMLIT_T, STRLIT_T, SQUOTE_T};

  switch (token){

    case LPAREN_T: //50  <any_other_token> -> LPAREN_T <more_tokens> RPAREN_T
    P2 << "Using Rule 50" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    code.pop_back();
    errors += more_tokens();
    code += lex->GetLexeme();
    if(token == RPAREN_T){
      token = lex->GetToken();
      debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    }
    else{
      errors++;
      lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
      debug2 << "error in more_tokens, unexpected: " << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    }
    break;

    case IDENT_T://51    <any_other_token> -> IDENT_T
    P2 << "Using Rule 51" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case NUMLIT_T://52    <any_other_token> -> NUMLIT_T
    P2 << "Using Rule 52" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case STRLIT_T://53    <any_other_token> -> STRLIT_T
    P2 << "Using Rule 53" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case LISTOP2_T://54    <any_other_token> -> LISTOP2_T
    P2 << "Using Rule 54" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case IF_T://55    <any_other_token> -> IF_T
    P2 << "Using Rule 55" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case DISPLAY_T://56    <any_other_token> -> DISPLAY_T
    P2 << "Using Rule 56" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case NEWLINE_T://57    <any_other_token> -> NEWLINE_T
    P2 << "Using Rule 57" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case LISTOP1_T://58    <any_other_token> -> LISTOP1_T
    P2 << "Using Rule 58" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case AND_T://59    <any_other_token> -> AND_T
    P2 << "Using Rule 59" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case OR_T://60    <any_other_token> -> OR_T
    P2 << "Using Rule 60" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case NOT_T: //61    <any_other_token> -> NOT_T
    P2 << "Using Rule 61" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case DEFINE_T://62    <any_other_token> -> DEFINE_T
    P2 << "Using Rule 62" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case NUMBERP_T://63 <any_other_token> -> NUMBERP_T
    P2 << "Using Rule 63" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case LISTP_T://64    <any_other_token> -> LISTP_T
    P2 << "Using Rule 64" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case ZEROP_T: //65    <any_other_token> -> ZEROP_T
    P2 << "Using Rule 65" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case NULLP_T://66    <any_other_token> -> NULLP_T
    P2 << "Using Rule 66" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case STRINGP_T: //67    <any_other_token> -> STRINGP_T
    P2 << "Using Rule 67" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case PLUS_T://68    <any_other_token> -> PLUS_T
    P2 << "Using Rule 68" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case MINUS_T://69    <any_other_token> -> MINUS_T
    P2 << "Using Rule 69" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case DIV_T: //70    <any_other_token> -> DIV_T
    P2 << "Using Rule 70" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case MULT_T: //71    <any_other_token> -> MULT_T
    P2 << "Using Rule 71" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case MODULO_T: //72    <any_other_token> -> MODULO_T
    P2 << "Using Rule 72" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case ROUND_T://73    <any_other_token> -> ROUND_T
    P2 << "Using Rule 73" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case EQUALTO_T: //74    <any_other_token> -> EQUALTO_T
    P2 << "Using Rule 74" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case GT_T://75    <any_other_token> -> GT_T
    P2 << "Using Rule 75" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case LT_T://76    <any_other_token> -> LT_T
    P2 << "Using Rule 76" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case GTE_T: //77    <any_other_token> -> GTE_T
    P2 << "Using Rule 77" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case LTE_T: //78    <any_other_token> -> LTE_T
    P2 << "Using Rule 78" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case SQUOTE_T: //79    <any_other_token> -> SQUOTE_T <any_other_token>
    P2 << "Using Rule 79" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();

    errors += any_other_token();
    break;

    case COND_T: //80    <any_other_token> -> COND_T
    P2 << "Using Rule 80" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    case ELSE_T: //81    <any_other_token> -> ELSE_T
    P2 << "Using Rule 81" << endl;
    debug2 << lex->GetTokenName(token) << " " << lex -> GetLexeme()<<endl;
    token = lex->GetToken();
    break;

    default:
    errors++;
    debug2 << "Error in any_other_token! unexpected: " << lex->GetTokenName(token) << endl;
    lex-> ReportError("unexpected '" + lex->GetLexeme() + "'");
    break;
  }
  return errors;
}
