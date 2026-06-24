#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "symtb.h"

extern char symtb_trace;

static node *top = NULL;

static node **save = NULL;
static unsigned long int num_of_nodes = 0;

void push_tb(void)
{
    node *temp = malloc(sizeof(node));
    temp->entry = NULL;
    temp->num_of_entry = 0;

    if(top == NULL)
        temp->level = 0;
    else
        temp->level = top->level + 1;

    temp->next = top;
    top = temp;

    num_of_nodes++;
    save = realloc(save, num_of_nodes * sizeof(node));
    save[num_of_nodes - 1] = top;
}

void pop_tb(void)
{
    if(top == NULL)
    {
        printf("ERROR! Unable to pop table because stack is empty.\n");
        exit(-1);
    }

    top = top->next;
}

void create_entry(token tok, ident_type type)
{
	unsigned long int i;
	for(i = top->num_of_entry; i >= 0 && top->entry != NULL; i--)
	{
		if(tok.length == top->entry[i].length)
			if(strncmp(tok.sym, top->entry[i].sym, tok.length) == 0)
				printf("TYPE ERROR! Redeclaration of ("), prt_sym(tok), printf(")\n"), exit(-1);
		
		if(!i) break;
	}
	
    top->num_of_entry++;
    top->entry = realloc(top->entry, top->num_of_entry * sizeof(entry));
    top->entry[top->num_of_entry - 1].sym = tok.sym;
    top->entry[top->num_of_entry - 1].length = tok.length;
    top->entry[top->num_of_entry - 1].type = type;
}
/*

for later:

int search_sym(char *sym)
{
	node *temp;
	
	for(temp = top; temp != NULL; temp = temp->next)
	{
		unsigned long int i;
		for(i = temp->num_of_entry; i >= 0; i--)
		{
			if(strlen(sym) == strlen(temp->entry[i].sym))
				if(strncmp(sym, temp->entry[i].sym, temp->entry[i].length) == 0)
					return 1;
				
			if(!i) break;
		}
	}
	
	return 0;
}

*/

void trace_symbols(void)
{
    long int i, most_nested = 0, current_level;
	
	if(symtb_trace)
	{
		for(i = 0; i < num_of_nodes; i++)
			most_nested = save[i]->level > most_nested ? save[i]->level : most_nested;


		for(current_level = 0; current_level <= most_nested; current_level++)
		{
			unsigned long int tb_per_level = 0;

			printf("nest level %d:\n", current_level);

			for(i = 0; i < num_of_nodes; i++)
			{
				if(current_level == save[i]->level)
				{
					printf("table %lu:\n", tb_per_level);
					tb_per_level++;

					if(save[i]->entry != NULL)
					{
						unsigned long int j;
						for(j = 0; j < save[i]->num_of_entry; j++)
						{
							unsigned long int k;
							
							printf("\t(");
							for(k = 0; k < save[i]->entry[j].length; k++) printf("%c", save[i]->entry[j].sym[k]);
							printf(", ");
							

							switch(save[i]->entry[j].type)
							{
								case idt_const: printf("idt_const)\n"); break;
								case idt_type: printf("idt_type)\n"); break;
								case idt_variable: printf("idt_variable)\n"); break;
								case idt_field: printf("idt_field)\n"); break;
								case idt_proc: printf("idt_proc)\n"); break;
								case idt_func: printf("idt_func)\n"); break;
							}
						}
					} else
						printf("\t(NULL)\n");
				}
			}

			printf("\n\n");
		}
	}

    for(i = 0; i < num_of_nodes; i++)
    {
        /* these free each node and all entries under that node */
        free(save[i]->entry);
        free(save[i]);
    }

    free(save);
}