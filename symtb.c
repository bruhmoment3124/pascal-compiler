#include <stdio.h>
#include <stdlib.h>
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

void create_entry(char *sym, ident_type type)
{
    top->num_of_entry++;
    top->entry = realloc(top->entry, top->num_of_entry * sizeof(entry));
    top->entry[top->num_of_entry - 1].sym = sym;
    top->entry[top->num_of_entry - 1].type = type;
}

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
							printf("\t(%s, ", save[i]->entry[j].sym);

							switch(save[i]->entry[j].type)
							{
								case var: printf("var)\n"); break;
								case id_type: printf("id_type)\n"); break;
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