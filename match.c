#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "symtb.h"
#include "match.h"

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
	
	if(temp.length != strlen(sym))
		return 0;
	
	if(strncmp(temp.sym, sym, temp.length) != 0)
		return 0;

	return 1;
}

int match_type(type t)
{
	token temp = get_tok(stay);
	
	if(temp.type == t) return 1;
	
	return 0;
}

/* match a specific identifier type in the input */
int match_idt(ident_type t)
{
	token tok = get_tok(stay);
	
	if(match_type(identifier))
	{
		entry *temp_ent = search_sym(tok);
		
		if(temp_ent != NULL)
			if(temp_ent->type == t)
				return 1;
	}

	return 0;
}

/* like match_sym, but advances the input and throw a syntax error if sym is not in the input file  */
void expect_sym(char *sym)
{	
	if(trace)
	{
		unsigned long int i;
		for(i = 0; i < indent; i++) printf("| ");	
		printf("expect_sym (%s)\n", sym);
	}
	
	if(!match_sym(sym))
		syntax_error(sym, get_tok(stay));
	
	get_tok(advance);
}

/* like expect_sym, but for symbol type */
void expect_type(type t)
{
	token temp = get_tok(advance);
	
	if(trace)
	{
		unsigned long int i;
		for(i = 0; i < indent; i++) printf("| ");

		printf("expect_type (");
		prt_type(temp);
		printf(")\n");
	}
	
	if(temp.type != t)
		syntax_error("TYPE", temp);
}

void expect_idt(ident_type t)
{
	if(trace)
	{
		unsigned long int i;
		for(i = 0; i < indent; i++) printf("| ");

		printf("expect_idt (");
		
		switch(t)
		{
			case idt_const: printf("idt_const)\n"); break;
			case idt_type: printf("idt_type)\n"); break;
			case idt_variable: printf("idt_variable)\n"); break;
			case idt_field: printf("idt_field)\n"); break;
			case idt_bound: printf("idt_bound)\n"); break;
			case idt_proc: printf("idt_proc)\n"); break;
			case idt_func: printf("idt_func)\n"); break;
		}
	}
	
	if(!match_idt(t))
		syntax_error("idt", get_tok(stay));
	
	get_tok(advance);
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