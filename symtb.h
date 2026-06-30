
typedef enum
{
	idt_const,
	idt_type,
	idt_variable,
	idt_field,
	idt_bound,
	idt_proc,
	idt_func
} ident_type;

typedef struct
{
	char *sym;
	unsigned long int length;
	ident_type type;
} entry;

typedef struct node
{
	entry *entry;
	unsigned long int num_of_entry;
	long int level;
	struct node *next;
} node;

void push_tb(void);
void pop_tb(void);

void create_entry(token, ident_type);

entry * search_sym(token);

void trace_symbols(void);