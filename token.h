
typedef enum
{
	unknown,
	special,
	word,
	identifier,
	uint,
	unum,
	string,
	EOI
} type;

typedef struct
{
	char *sym;
	unsigned long int length, lines; /* symbol length and number of lines */
	type type;
} token;

typedef enum
{
	stay,
	advance
} choice;

void rewind_pos(void);

token get_tok(choice);

void prt_sym(token);
void prt_type(token);