#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "match.h"
#include "parser.h"

extern char *file; /* the string that the file was converted into */
extern char trace;

/* 
	The below functions are for the parsing stage of the compiler.
*/

static unsigned long int indent = 0; /* used to format traced output */

/* boilerplate sytnax_error, will change later */
static void syntax_error(char *sym, token temp)
{
	/* for whatever reason, temp.lines seems to be broken; I need to investigate */
	printf("ERROR! On line # %lu: expected (%s), got (", temp.lines, sym);
	prt_sym(temp);
	printf(")\n");
	
	exit(-1);
}

/* 
	Function prologue and epilogue; when trace is true,
	prologue and epilogue are executed and the parsers actions
	are traced.
*/

void prologue(char *str)
{
	if(trace)
	{
		unsigned long int i;
		for(i = 0; i < indent; i++) printf("| ");
		printf("%s {\n", str);
		indent++;
	}
}

void epilogue(void)
{
	if(trace)
	{
		unsigned long int i;
		indent--;
		for(i = 0; i < indent; i++) printf("| ");
		printf("}\n");
	}
}

/* functions used in a standard recursive descent parser */

int match_sym(char *sym)
{
	token temp = get_tok(stay);
	
	unsigned long int size, i, j;
	
	/* find size of sym */
	for(size = 0; sym[size] != '\0'; size++);
	
	/* if the two symbols are not the same size, they cannot be the same */
	if(temp.end - temp.start != size)
		return 0;
	
	/* verify that all characters in the two symbols are the same */
	for(i = temp.start, j = 0; i < temp.end; i++, j++)
		if(file[i] != sym[j]) return 0;

	return 1;
}

int match_type(type t)
{
	token temp = get_tok(stay);
	
	if(temp.type == t) return 1;
	
	return 0;
}

/* like match_sym, but advances the input and throw a syntax error if sym is not in the input file  */
void expect_sym(char *sym)
{	
	token temp = get_tok(advance);
	
	unsigned long int size, i, j;

	if(trace)
	{
		unsigned long int i;
		for(i = 0; i < indent; i++) printf("| ");	
		printf("expect (%s)\n", sym);
	}
	
	/* find size of sym */
	for(size = 0; sym[size] != '\0'; size++);
	
	/* if the two symbols are not the same size, they cannot be the same */
	if(temp.end - temp.start != size)
		syntax_error(sym, temp);
	
	/* verify that all characters in the two symbols are the same */
	for(i = temp.start, j = 0; i < temp.end; i++, j++)
		if(file[i] != sym[j]) syntax_error(sym, temp);
}

/* like expect_sym, but for symbol type */
void expect_type(type t)
{
	token temp = get_tok(advance);
	
	if(trace)
	{
		unsigned long int i;
		for(i = 0; i < indent; i++) printf("| ");

		printf("expect (");
		prt_type(temp);
		printf(")\n");
	}
	
	if(temp.type != t)
		syntax_error("TYPE", temp);
}

/* used in the case a match is found */
void advance_sym(void)
{
	token temp = get_tok(advance);
	
	if(trace)
	{
		unsigned long int i;
		for(i = 0; i < indent; i++) printf("| ");
		
		printf("advance (");
		prt_sym(temp);
		printf(")\n");
	}
}