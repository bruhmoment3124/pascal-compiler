#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "match.h"
#include "parser.h"

/*
	Below is the recursive descent parser.
*/

/*

Below is the basic parsing of expressions:

void factor(void)
{
	prologue("factor");
	
	if(match_type(uint) || match_type(unum))
		expect_type(get_tok(stay).type_pasc);
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

*/

void constant(void)
{
	prologue("constant");
	
	if(match_sym("+") || 
	   match_sym("-") ||
	   match_type(identifier) || 
	   match_type(unum) || 
	   match_type(uint))
	{
		if(match_sym("+") || match_sym("-"))
			advance_sym();
			
		if(match_type(identifier) || 
		   match_type(unum) || 
	       match_type(uint))
		{
			advance_sym();
		} else
		{
			/* replace with standard syntax error function */
			printf("ERROR!\n");
			exit(-1);
		}
	} else if(match_type(string))
	{
		advance_sym();
	} else
	{
		/* replace with standard syntax error function */
		printf("ERROR!\n");
		exit(-1);
	}
	
	epilogue();
}

void index_type_spec(void)
{
	prologue("index_type_spec");
	
	expect_type(identifier);
	expect_sym("..");
	expect_type(identifier);
	expect_sym(":");
	
	/* type identifier */
	expect_type(identifier);
	
	epilogue();
}

void conformant_arr(void)
{
	prologue("conformant_arr");
	
	if(match_sym("packed"))
	{
		expect_sym("packed");
		expect_sym("array");
		expect_sym("[");
		index_type_spec();
		expect_sym("]");
		expect_sym("of");
		
		/* type identifier */
		expect_type(identifier);
	} else if(match_sym("array"))
	{
		expect_sym("array");
		expect_sym("[");
		
		index_type_spec();
		
		while(match_sym(";"))
		{
			expect_sym(";");
			index_type_spec();
		}
		
		expect_sym("]");
		expect_sym("of");
		
		if(match_sym("packed") || match_sym("array"))
		{
			conformant_arr(); /* recursion here because it's easier */
		} else if(match_type(identifier))
		{
			expect_type(identifier);
		} else
		{
			/* replace with standard syntax error function */
			printf("ERROR!\n");
			exit(-1);
		}
	}
	
	epilogue();
}

/* THIS IS ALMOST DONE, WAY TO VERBOSE, HOWEVER */
void formal_param_list(void)
{
	prologue("formal_param_list");
	
	expect_sym("(");
	
	if(match_sym("var"))
	{
		expect_sym("var");
		
		expect_type(identifier);
		while(match_sym(","))
		{
			expect_sym(",");
			expect_type(identifier);
		}
		
		expect_sym(":");
		
		if(match_type(identifier))
		{
			expect_type(identifier);
		} else if(match_sym("packed") || match_sym("array"))
		{
			conformant_arr();
		} else
		{
			/* replace with standard syntax error function */
			printf("ERROR!\n");
			exit(-1);
		}
	} else if(match_type(identifier))
	{
		expect_type(identifier);
		while(match_sym(","))
		{
			expect_sym(",");
			expect_type(identifier);
		}
		
		expect_sym(":");
		
		if(match_type(identifier))
		{
			expect_type(identifier);
		} else if(match_sym("packed") || match_sym("array"))
		{
			conformant_arr();
		} else
		{
			/* replace with standard syntax error function */
			printf("ERROR!\n");
			exit(-1);
		}
	} else if(match_sym("procedure") || match_sym("function"))
	{
		proc_or_func_head();
	} else
	{
		/* replace with standard syntax error function */
		printf("ERROR!\n");
		exit(-1);
	}
	
	while(match_sym(";"))
	{
		expect_sym(";");
		
		if(match_sym("var"))
		{
			expect_sym("var");
			
			expect_type(identifier);
			while(match_sym(","))
			{
				expect_sym(",");
				expect_type(identifier);
			}
			
			expect_sym(":");
			
			if(match_type(identifier))
			{
				expect_type(identifier);
			} else if(match_sym("packed") || match_sym("array"))
			{
				conformant_arr();
			} else
			{
				/* replace with standard syntax error function */
				printf("ERROR!\n");
				exit(-1);
			}
		} else if(match_type(identifier))
		{
			expect_type(identifier);
			while(match_sym(","))
			{
				expect_sym(",");
				expect_type(identifier);
			}
			
			expect_sym(":");
			
			if(match_type(identifier))
			{
				expect_type(identifier);
			} else if(match_sym("packed") || match_sym("array"))
			{
				conformant_arr();
			} else
			{
				/* replace with standard syntax error function */
				printf("ERROR!\n");
				exit(-1);
			}
		} else if(match_sym("procedure") || match_sym("function"))
		{
			proc_or_func_head();
		} else
		{
			/* replace with standard syntax error function */
			printf("ERROR!\n");
			exit(-1);
		}
	}
	
	expect_sym(")");
	
	epilogue();
}

/* UNFINISHED */
void proc_or_func_head(void)
{
	prologue("proc_or_func_head");
	
	if(match_sym("procedure"))
	{
		expect_sym("procedure");
		expect_type(identifier);
		
		if(match_sym("("))
			formal_param_list();
	} else if(match_sym("function"))
	{
		expect_sym("function");
		expect_type(identifier);
		
		if(match_sym("("))
			formal_param_list();
		
		if(match_sym(":"))
		{
			expect_sym(":");
			expect_type(identifier);
		}
	} else
	{
		/* replace with standard syntax error function */
		printf("ERROR!\n");
		exit(-1);
	}
	
	epilogue();
}

/* UNFINISHED TYPE FUNCTION, WILL COMPLETE LATER */
void type_pasc(void)
{
	prologue("type_pasc");
	
	expect_sym("type");
	
	epilogue();
}

void block(void)
{
	prologue("block");
	
	if(match_sym("label"))
	{
		expect_sym("label");
		expect_type(uint);
		
		while(match_sym(","))
		{
			expect_sym(",");
			expect_type(uint);
		}
		
		expect_sym(";");
	}
	
	if(match_sym("const"))
	{
		expect_sym("const");
		
		do
		{
			expect_type(identifier);
			expect_sym("=");
			constant();
			expect_sym(";");
		} while(match_type(identifier));
	}
	
	if(match_sym("type_pasc"))
	{
		expect_sym("type_pasc");
		
		do
		{
			expect_type(identifier);
			expect_sym("=");
			type_pasc();
			expect_sym(";");
		} while(match_type(identifier));
	}
	
	if(match_sym("var"))
	{
		expect_sym("var");
		
		do
		{
			expect_type(identifier);
			
			while(match_sym(","))
			{
				expect_sym(",");
				expect_type(identifier);
			}
			
			expect_sym(":");
			
			type_pasc();
			
			expect_sym(";");
		} while(match_type(identifier));
	}
	
	while(match_sym("procedure") || match_sym("function"))
	{
		proc_or_func_head();
		expect_sym(";");
		
		/* directive or block */
		if(match_sym("forward")) expect_sym("forward");
		else block();
	
		expect_sym(";");
	}
	
	expect_sym("begin");
	
	/* statement  */
	/* while ; statement */
	
	expect_sym("end");
	
	epilogue();
}

void program(void)
{
	prologue("program");
	
	expect_sym("program");
	expect_type(identifier);
	
	if(match_sym("("))
	{
		expect_sym("(");
		
		expect_type(identifier);
		
		while(match_sym(","))
		{
			expect_sym(",");
			expect_type(identifier);
		}
		
		expect_sym(")");
	}
	
	expect_sym(";");
	block();
	expect_sym(".");
	
	epilogue();
}