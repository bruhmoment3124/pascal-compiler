#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "match.h"
#include "parser.h"
#include "symtb.h"

char *file = NULL; /* string containing input symbols to scanner */
char trace = 0; /* trace parsing? */
char symtb_trace = 0; /* print all found symbol tables */

/* read file into string, with ~ as eof */
void file2str(FILE *source)
{
	unsigned long int size = 0;
	
	char ch;
	while(ch = fgetc(source), !feof(source))
	{
		size++;
		file = realloc(file, size + 2);
		file[size - 1] = ch;
		file[size] = '~';
		file[size + 1] = '\0';
	}
	
	if(size == 0)
	{
		file = realloc(file, size + 2);
		file[0] = '~';
		file[1] = '\0';
	}
}

int main(int argc, char *argv[])
{
	FILE *source = fopen("source.txt", "r");
	file2str(source);
	
	/* handle command line arguments, NOT DONE */
	if(argc - 1)
	{
		unsigned char i;
		for(i = 1; i < argc; i++)
		{
			unsigned char size;
			for(size = 0; argv[i][size] != '\0'; size++);
		
			if(argv[i][0] == '-' && size == 2)
			{
				token temp_tok; /* variable to hold each token */
				
				switch(argv[i][1])
				{
					case 's':
						printf("token output:\n\n");
						
						do
						{	
							temp_tok = get_tok(advance);
							printf("%lu: (", temp_tok.lines);
							
							prt_sym(temp_tok);
							printf(", ");
							prt_type(temp_tok);
							
							printf(")\n");
						} while(temp_tok.type != EOI);
						
						printf("\n");
						
						rewind_pos();
					break;
					
					case 't':
						trace = 1;
					break;
					
					case 'l':
						symtb_trace = 1;
					break;
				}
			} else
			{
				printf("ERROR! incorrectly formed argument \"%s\".\n", argv[i]);
				exit(-1);
			}
		}
	}
	
	if(trace) printf("trace output:\n\n");
	
	expression();
	expect_sym("~");
	printf("\n");
	
	if(symtb_trace) printf("symbol table trace:\n\n");
	trace_symbols();
	
	free(file);
	fclose(source);
	
	return 0;
}