
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
	unsigned long int start, end, lines; /* symbol components and number of lines */
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