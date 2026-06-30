#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "symtb.h"
#include "match.h"
#include "inner.h"

void unsigned_constant(void)
{
	if(match_idt(idt_const))
	{
		expect_idt(idt_const);
	} else if(match_type(uint) || match_type(unum))
	{
		expect_type(get_tok(stay).type);
	} else if(match_sym("nil"))
	{
		expect_sym("nil");
	} else if(match_type(string))
	{
		expect_type(string);
	} else
	{
		printf("ERROR! Expected an unsigned constant.\n");
		exit(-1);
	}
}

void variable(void)
{
	prologue("variable");
	
	expect_type(identifier);
	
	while(match_sym("[") || match_sym(".") || match_sym("^"))
	{
		if(match_sym("["))
		{
			expect_sym("[");
			
			expression();
			
			while(match_sym(","))
			{
				expect_sym(",");
				expression();
			}
			
			expect_sym("]");
		} else if(match_sym("."))
		{
			expect_sym(".");
			expect_type(identifier);
		} else
		{
			expect_sym("^");
		}
	}
	
	epilogue();
}

/* factor is MUCH closer to done, symbol overloading still needs to be implemented, however */
void factor(void)
{
	prologue("factor");
	
	if(match_idt(idt_const) || 
		match_type(uint)	|| 
		match_type(unum)	||
		match_sym("nil")	||
		match_type(string))
	{
		unsigned_constant();
	} else if(match_idt(idt_bound))
	{
		expect_idt(idt_bound);
	} else if(match_idt(idt_variable) || match_idt(idt_field))
	{
		variable();
	} else if(match_idt(idt_func))
	{
		expect_idt(idt_func);
		
		if(match_sym("("))
			actual_param_list();
	} else if(match_sym("("))
	{
		expect_sym("(");
		expression();
		expect_sym(")");
	} else if(match_sym("not"))
	{
		expect_sym("not");
		factor();
	} else if(match_sym("["))
	{
		expect_sym("[");
		
		if(match_sym("]"))
		{
			expect_sym("]");
		} else
		{
			expression();
			
			if(match_sym(".."))
			{
				expect_sym("..");
				expression();
			}
			
			while(match_sym(","))
			{
				expect_sym(",");
				
				expression();
			
				if(match_sym(".."))
				{
					expect_sym("..");
					expression();
				}
			}
			
			expect_sym("]");
		}
	} else
	{
		/* replace with standard syntax error function */
		printf("ERROR! Expected factor.\n");
		exit(-1);
	}
	
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
	
	if(match_sym("+") || match_sym("-"))
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

/* both param lists need to be implemented */

void actual_param_list(void)
{
	prologue("actual_param_list");
	
	epilogue();
}

void write_param_list(void)
{
	prologue("write_param_list");
	
	epilogue();
}

void statement(void)
{
	prologue("statement");
	
	if(match_sym("begin"))
	{
		expect_sym("begin");
	
		statement();
		while(match_sym(";"))
		{
			expect_sym(";");
			statement();
		}
		
		expect_sym("end");
		
	/* MUST VERIFY THAT DANGLING ELSE IS HANDLED PROPERLY! */
	} else if(match_sym("if"))
	{
		expect_sym("if");
		
		/* expression */
		expect_sym("e");
		
		expect_sym("then");
		statement();
		
		if(match_sym("else"))
		{
			expect_sym("else");
			statement();
		}
	} else if(match_sym("case"))
	{
		expect_sym("case");
		
		/* expression */
		expect_sym("e");
		
		expect_sym("of");
		
		do
		{
			constant();
			while(match_sym(","))
			{
				expect_sym(",");
				constant();
			}
			
			expect_sym(":");
			statement();
			
			if(match_sym("end"))
				break;
			
			expect_sym(";");
			
			if(match_sym("end"))
				break;
		/* while "constant" */
		} while(match_idt(idt_const) || 
				match_sym("+")   	 || 
				match_sym("-")   	 ||
				match_type(unum) 	 || 
				match_type(uint)	 ||
				match_type(string));
		
		expect_sym("end");
	} else if(match_sym("while"))
	{
		expect_sym("while");
		
		/* expression */
		expect_sym("e");
		
		expect_sym("do");
		statement();
	} else if(match_sym("repeat"))
	{
		expect_sym("repeat");
		
		statement();
		while(match_sym(";"))
		{
			expect_sym(";");
			statement();
		}
		
		expect_sym("until");
		
		/* expression */
		expect_sym("e");
	} else if(match_sym("for"))
	{
		expect_sym("for");
		
		/* variable identifier */
		expect_sym("v");
		
		expect_sym(":=");
		
		/* expression */
		expect_sym("e");
		
		if(match_sym("downto") || match_sym("to"))
			advance_sym();
		else
		{
			printf("ERROR! Expected 'downto' or 'to'.\n");
			exit(-1);
		}
		
		/* expression */
		expect_sym("e");
		
		expect_sym("do");
		statement();
	} else if(match_sym("with"))
	{
		expect_sym("with");
		
		/* variable */
		expect_sym("v");
		
		while(match_sym(","))
		{
			expect_sym(",");
				
			/* variable */
			expect_sym("v");
		}
		
		expect_sym("do");
		statement();
	} else if(match_sym("goto"))
	{
		expect_sym("goto");
		expect_type(uint);
	} /* no else error because empty statement is possible */
	
	epilogue();
}