
void prologue(char *);
void epilogue(void);

int match_sym(char *);
int match_type(type);
int match_idt(ident_type);

void expect_sym(char *);
void expect_type(type);
void expect_idt(ident_type);

void advance_sym(void);