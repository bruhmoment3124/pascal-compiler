
typedef enum
{
    var,
    id_type
} ident_type;

typedef struct
{
    char *sym;
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

void create_entry(char *, ident_type);

void trace_symbols(void);