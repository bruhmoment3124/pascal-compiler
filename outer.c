#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "symtb.h"
#include "match.h"
#include "outer.h"
#include "inner.h"

/*
	Below is the recursive descent parser.
*/

void constant(void)
{
	prologue("constant");
	
	if(match_idt(idt_const))
	{
		expect_idt(idt_const);
	} else if(match_sym("+")   || 
			  match_sym("-")   ||
			  match_type(unum) || 
			  match_type(uint))
	{
		if(match_sym("+") || match_sym("-"))
			advance_sym();
		
		advance_sym();
	} else if(match_type(string))
	{
		advance_sym();
	} else
	{
		/* replace with standard syntax error function */
		printf("ERROR! Expected constant identifier or unsigned number.\n");
		exit(-1);
	}
	
	epilogue();
}

void index_type_spec(void)
{
	prologue("index_type_spec");
	
	create_entry(get_tok(stay), idt_bound);
	expect_type(identifier);
	expect_sym("..");
	create_entry(get_tok(stay), idt_bound);
	expect_type(identifier);
	expect_sym(":");
	expect_idt(idt_type);
	
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
		expect_idt(idt_type);
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
		} else if(match_idt(idt_type))
		{
			expect_idt(idt_type);
		} else
		{
			/* replace with standard syntax error function */
			printf("ERROR!\n");
			exit(-1);
		}
	}
	
	epilogue();
}

/* CLOSER TO DONE, STILL NEED TO VERIFY */
void formal_param_list(void)
{
	char first = 1; /* test for first iteration of the loop, important to reduce size */
	
	prologue("formal_param_list");
	
	expect_sym("(");
	
	do
	{
		/* only expect semicolon at the beginning of subsequent iterations, not the first */
		if(!first)
			expect_sym(";");
		first = 0;
		
		if(match_sym("var"))
		{
			expect_sym("var");
			
			create_entry(get_tok(stay), idt_variable);
			expect_type(identifier);
			
			while(match_sym(","))
			{
				expect_sym(",");		
				
				create_entry(get_tok(stay), idt_variable);
				expect_type(identifier);
			}
			
			expect_sym(":");
			
			if(match_idt(idt_type))
			{
				expect_idt(idt_type);
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
			create_entry(get_tok(stay), idt_variable);
			expect_type(identifier);
			
			while(match_sym(","))
			{
				expect_sym(",");
				
				create_entry(get_tok(stay), idt_variable);
				expect_type(identifier);
			}
			
			expect_sym(":");
			
			if(match_idt(idt_type))
			{
				expect_idt(idt_type);
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
		
	/* while condition is the follow set of the inside of the parentheses */
	} while(!match_sym(")"));
	
	expect_sym(")");
	
	epilogue();
}

void proc_or_func_head(void)
{
	prologue("proc_or_func_head");
	
	if(match_sym("procedure"))
	{
		expect_sym("procedure");
		
		create_entry(get_tok(stay), idt_proc);
		expect_type(identifier);
		
		push_tb();
		
		if(match_sym("("))
			formal_param_list();
	} else if(match_sym("function"))
	{
		expect_sym("function");
		
		create_entry(get_tok(stay), idt_func);
		expect_type(identifier);
		
		push_tb();
		
		if(match_sym("("))
			formal_param_list();
		
		if(match_sym(":"))
		{
			expect_sym(":");
			expect_idt(idt_type);
		}
	} else
	{
		/* replace with standard syntax error function */
		printf("ERROR!\n");
		exit(-1);
	}
	
	epilogue();
}

void ordinal_type(void)
{
	prologue("ordinal_type");
	
	if(match_idt(idt_type))
	{
		expect_idt(idt_type);
	} else if(match_sym("("))
	{
		expect_sym("(");
		
		create_entry(get_tok(stay), idt_const);
		expect_type(identifier);
		
		while(match_sym(","))
		{
			expect_sym(",");	
			
			create_entry(get_tok(stay), idt_const);
			expect_type(identifier);
		}
		
		expect_sym(")");
	} else
	{
		constant();
		expect_sym("..");
		constant();
	}
	
	epilogue();
}

void data_type(void)
{
	prologue("data_type");
	
	if(match_sym("^"))
	{
		expect_sym("^");
		expect_idt(idt_type);
	} else if(match_sym("packed") ||
			  match_sym("array") ||
			  match_sym("file") ||
			  match_sym("set") ||
			  match_sym("record"))
	{
		if(match_sym("packed"))
			expect_sym("packed");
		
		if(match_sym("array"))
		{
			expect_sym("array");
			expect_sym("[");
			
			ordinal_type();
			
			while(match_sym(","))
			{
				expect_sym(",");
				ordinal_type();
			}
			
			expect_sym("]");
			expect_sym("of");
			data_type();
		} else if(match_sym("file"))
		{
			expect_sym("file");
			expect_sym("of");
			data_type();
		} else if(match_sym("set"))
		{
			expect_sym("file");
			expect_sym("of");
			ordinal_type();
		} else
		{
			expect_sym("record");
			field_list();		
			expect_sym("end");
		}
	} else
	{
		ordinal_type();
	}
	
	epilogue();
}

/* field list is WAY better now, still have got to test it more, though */
void field_list(void)
{
	prologue("field_list");
	
	push_tb();
	
	if(match_type(identifier))
	{	
		do
		{
			create_entry(get_tok(stay), idt_field);
			expect_type(identifier);
			
			while(match_sym(","))
			{
				expect_sym(",");
				create_entry(get_tok(stay), idt_field);
				expect_type(identifier);
			}
			
			expect_sym(":");
			data_type();
			
			/* follow set of field_list */
			if(match_sym("end") || match_sym(")")) break;
			
			expect_sym(";");
		} while(match_type(identifier));
	}
	
	if(match_sym("case"))
	{
		expect_sym("case");
		
		if(match_idt(idt_type))
		{
			expect_idt(idt_type);
		} else
		{
			expect_type(identifier);
			expect_sym(":");
			expect_idt(idt_type);
		}
		
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
			expect_sym("(");
			field_list();
			expect_sym(")");
			
			/* follow set of field_list */
			if(match_sym("end") || match_sym(")")) break;
			
			expect_sym(";");
			
		/* while "constant" */
		} while(match_idt(idt_const) || 
				match_sym("+")   	 || 
				match_sym("-")   	 ||
				match_type(unum) 	 || 
				match_type(uint)	 ||
				match_type(string));
	}
	
	pop_tb();
	
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
			create_entry(get_tok(stay), idt_const);
			expect_type(identifier);
			expect_sym("=");
			constant();
			expect_sym(";");
		} while(match_type(identifier));
	}
	
	if(match_sym("type"))
	{
		token *pt_type_list = NULL;
		unsigned long int list_length = 0;
		
		expect_sym("type");
		
		do
		{
			/* saving identifier for later create_entry */
			token temp = get_tok(stay);
			
			expect_type(identifier);
			expect_sym("=");
			
			/* distinction here because pointers can be used before declaration */
			if(match_sym("^"))
			{
				expect_sym("^");
				
				/* allocate space for token */
				list_length++;
				pt_type_list = realloc(pt_type_list, list_length * sizeof(token));
				pt_type_list[list_length - 1] = get_tok(stay);
				
				expect_type(identifier);
			} else
			{
				data_type();
			}
			
			expect_sym(";");
		
			/* create entry here so you can't define type as itself */
			create_entry(temp, idt_type);
		} while(match_type(identifier));
		
		/* verify that the symbol used in a pointer type definition has been declared  */
		{
			unsigned long int i;
			for(i = 0; i < list_length; i++)
			{
				if(!search_sym(pt_type_list[i]))
					printf("ERROR! Pointer type identifier not resolved.\n"),
					exit(-1);
			}
		}
		
		free(pt_type_list);
	}
	
	if(match_sym("var"))
	{
		expect_sym("var");
		
		do
		{
			create_entry(get_tok(stay), idt_variable);
			expect_type(identifier);
			
			while(match_sym(","))
			{
				expect_sym(",");
				create_entry(get_tok(stay), idt_variable);
				expect_type(identifier);
			}
			
			expect_sym(":");
			
			data_type();
			
			expect_sym(";");
		} while(match_type(identifier));
	}
	
	while(match_sym("procedure") || match_sym("function"))
	{
		proc_or_func_head();
		expect_sym(";");
		
		/* directive or block */
		if(match_sym("forward")) expect_sym("forward"), pop_tb();
		else block();
	
		expect_sym(";");
	}
	
	expect_sym("begin");
	
	statement();
	while(match_sym(";"))
	{
		expect_sym(";");
		statement();
	}
	
	expect_sym("end");
	
	pop_tb();
	
	epilogue();
}

void program(void)
{
	prologue("program");
	
	push_tb(); /* initial symbol table push */
	
	expect_sym("program");
	
	create_entry(get_tok(stay), idt_variable);
	expect_type(identifier);
	
	if(match_sym("("))
	{
		expect_sym("(");
			
		create_entry(get_tok(stay), idt_variable);
		expect_type(identifier);
		
		while(match_sym(","))
		{
			expect_sym(",");
			
			create_entry(get_tok(stay), idt_variable);
			expect_type(identifier);
		}
		
		expect_sym(")");
	}
	
	/* predeclaring a number of identifier as needed in Pascal */
	
	/* only defining sym and length, because that's all that's needed for create_entry */
	{
		token boolean = {"boolean", 7},
			  real = {"real", 4},
			  integer = {"integer", 7}, 
			  character = {"char", 4},
			  true = {"true", 4},
			  false = {"false", 5};
		
		create_entry(boolean, idt_type);
		create_entry(real, idt_type);
		create_entry(integer, idt_type);
		create_entry(character, idt_type);
		
		/* technically a part of a enum */
		create_entry(true, idt_const);
		create_entry(false, idt_const);
	}
	
	expect_sym(";");
	block();
	expect_sym(".");
	
	/* symbol table is popped off as a part of block */
	
	epilogue();
}