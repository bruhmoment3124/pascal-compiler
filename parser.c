#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "match.h"
#include "parser.h"

/*
	Below is the recursive descent parser.
*/

/* NOT FINISHED */
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

void variable(void)
{
	prologue("variable");
	
	epilogue();
}

/* factor will be difficult, because it relies on type information */
void factor(void)
{
	prologue("factor");
	
	/* unsigned constant needs to be implemented */
	
	/* some work will have to be done to make LL(1) */
	if(match_type(identifier))
	{
		variable();
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
	} else if(match_type(unum) || match_type(uint))
	{
		expect_type(get_tok(stay).type);
	} else
	{
		/* replace with standard syntax error function */
		printf("ERROR!\n");
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

void write_param_list(void)
{
	prologue("write_param_list");
	
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

/* ALMOST DONE, NEED TO ENSURE VERIFY TYPE OF IDENTIFIER */
void ordinal_type(void)
{
	prologue("ordinal_type");
	
	if(match_type(identifier))
	{
		expect_type(get_tok(stay).type);
	} else if(match_sym("("))
	{
		expect_sym("(");
		
		expect_type(identifier);
		while(match_sym(","))
		{
			expect_sym(",");
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
		expect_type(identifier);
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

void field_list(void)
{
	prologue("field_list");
	
	/* no else because fields can be empty; no need to error correct */
	if(match_type(identifier))
	{
		/* 
			this variable is a little weird, but exists to properly parse field list;
			basically, case must be preceded by a semicolon, ONLY IF, previous identifier have been declared.
			There is probably a better way to do this, but idk.
		*/
		char case_after = 0;
		
		expect_type(identifier);
		
		while(match_sym(","))
		{
			expect_sym(",");
			expect_type(identifier);	
		}
		
		expect_sym(":");
		data_type();
		
		while(match_sym(";"))
		{
			expect_sym(";");
			
			/* end or ) or case must follow */
			if(match_sym("end") || match_sym(")")) break;
			
			if(match_sym("case"))
			{
				case_after = 1;
				break;
			}
			
			expect_type(identifier);
		
			while(match_sym(","))
			{
				expect_sym(",");
				expect_type(identifier);	
			}
			
			expect_sym(":");
			data_type();
		}
		
		if(case_after)
		{
			expect_sym("case");
			
			/* need to add distinction between identifier and type identifier */
			expect_type(identifier);
			
			if(match_sym(":"))
			{
				expect_sym(":");
				expect_type(identifier);
			}
			
			expect_sym("of");
			
			/* first instance, must repeat below */
			
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
			
			/* second instance */
			
			while(match_sym(";"))
			{	
				expect_sym(";");

				/* end or ) must follow a field list */
				if(match_sym("end") || match_sym(")")) break;
			
				/* need to add distinction between identifier and type identifier */
				
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
			}
		}
	} else if(match_sym("case"))
	{
		expect_sym("case");
		
		/* need to add distinction between identifier and type identifier */
		expect_type(identifier);
		
		if(match_sym(":"))
		{
			expect_sym(":");
			expect_type(identifier);
		}
		
		expect_sym("of");
		
		/* first instance, must repeat below */
		
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
		
		/* second instance */
		
		while(match_sym(";"))
		{	
			expect_sym(";");

			/* end or ) must follow a field list */
			if(match_sym("end") || match_sym(")")) break;
		
			/* need to add distinction between identifier and type identifier */
			
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
		}
	}
	
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
	
	if(match_sym("type"))
	{
		expect_sym("type");
		
		do
		{
			expect_type(identifier);
			expect_sym("=");
			data_type();
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
			
			data_type();
			
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