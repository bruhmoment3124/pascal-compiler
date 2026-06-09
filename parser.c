#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "match.h"
#include "parser.h"

/*
	Below is the recursive descent parser.
*/

void factor(void)
{
	prologue("factor");
	
	if(match_type(uint) || match_type(unum))
		expect_type(get_tok(stay).type);
	else
		printf("ERROR!\n"), exit(-1);
	
	epilogue();
}

void term(void)
{
	prologue("term");
	
	factor();
	
	while(match_sym("*") ||
		  match_sym("/") ||
		  match_sym("div") ||
		  match_sym("mod") ||
		  match_sym("and"))
	{
		advance_sym();
		factor();
	}
	
	epilogue();
}

void simple_expression(void)
{
	prologue("simple_expression");
	
	if(match_sym("+") || match_sym("+"))
		advance_sym();
	
	term();
	
	while(match_sym("+") ||
		  match_sym("-") ||
		  match_sym("or"))
	{
		advance_sym();
		term();
	}
	
	epilogue();
}

void expression(void)
{
	prologue("expression");
	
	simple_expression();
	
	if(match_sym("=") ||
	   match_sym("<") ||
	   match_sym(">") ||
	   match_sym("<>") ||
	   match_sym("<=") ||
	   match_sym(">=") ||
	   match_sym("in"))
	{
		advance_sym();
		simple_expression();
	}
	
	epilogue();
}