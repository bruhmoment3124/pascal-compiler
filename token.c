#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "token.h"

/* the below macros are used to scan the input as characters; 
   they are not related to later parsing stage functions */

/* returns true if ch1 and ch2 are equal */
#define match_ch(ch1, ch2) (ch1 == ch2)

/* return true if ch exists in set file */
#define match_set(ch, set) (strchr(set, ch) != NULL)

/* advance input */
#define advance(pos, length) pos++, length++

extern char *file;

static unsigned long int pos = 0, lines = 1;

void rewind_pos(void) { pos = 0; lines = 1; }

/* basic get_sym, need to add comment removal and make sure strings are properly handled */
token get_tok(choice c)
{
	token temp_tok;
	
	unsigned long int orig_pos = pos; /* used to revert position in case of stay */
	
	char whitespace[] = {
		' ', '\t', '\n',

		'\0'
	};

	/* the start of special_set symbols, the ends are delt with in the switch statement */
	char special_set[] = {
		'+', '-', '*', '/',
		'.', ',', ':', ';',
		'=', '<', '>', '^',
		'(', ')', '[', ']', '{', '}',
		
		'\'', /* beginning of string */
		'~', /* EOI symbol inserted by compiler */

		'\0'
	};

	/* remove whitespace from input, eventually comments as well */
	while(match_set(file[pos], whitespace))
	{
		if(match_ch(file[pos], '\n')) lines++;
		pos++;
	}
	
	temp_tok.sym = &file[pos];
	temp_tok.length = 0;
	temp_tok.lines = lines;
	temp_tok.type = unknown;

	/* if a special_set symbol is found */
	switch(file[pos])
	{
		/* likely will need to change when the removal of comments comes up */
		case '+': case '-': case '*': case '/':
		case ',': case ';': case '=': case '^':
		case '(': case ')': case '[': case ']': case '{': case '}':
			advance(pos, temp_tok.length);
			
			if(c == stay) pos = orig_pos;
			
			temp_tok.type = special;
			return temp_tok;
		break;

		/* end of file indicator, do not advance input */

		case '~':
			temp_tok.length++; /* ++ because no advance */
			if(c == stay) pos = orig_pos;
			
			temp_tok.type = EOI;
			return temp_tok;
		break;

		/* remove repetitions, if possible */
		
		case '.':
			advance(pos, temp_tok.length);
		
			if(match_ch(file[pos], '.'))
				advance(pos, temp_tok.length);
			
			if(c == stay) pos = orig_pos;
			
			temp_tok.type = special;
			return temp_tok;
		break;

		case ':':
			advance(pos, temp_tok.length);

			if(match_ch(file[pos], '='))
				advance(pos, temp_tok.length);
			
			if(c == stay) pos = orig_pos;
			
			temp_tok.type = special;
			return temp_tok;
		break;

		case '<':
			advance(pos, temp_tok.length);

			if(match_ch(file[pos], '>') || match_ch(file[pos], '='))
				advance(pos, temp_tok.length);
			
			if(c == stay) pos = orig_pos;
			
			temp_tok.type = special;
			return temp_tok;
		break;

		case '>':
			advance(pos, temp_tok.length);

			if(match_ch(file[pos], '='))
				advance(pos, temp_tok.length);
			
			if(c == stay) pos = orig_pos;
			
			temp_tok.type = special;
			return temp_tok;
		break;
	}

	/* if a string is found */
	if(match_ch(file[pos], '\''))
	{
		advance(pos, temp_tok.length);

		while(1)
		{
			if(file[pos] == '\'' && file[pos + 1] == '\'')
			{
				advance(pos, temp_tok.length);
			} else if(file[pos] == '\'' && file[pos + 1] != '\'')
				break;
			
			advance(pos, temp_tok.length);
		}
		
		advance(pos, temp_tok.length);
		
		if(c == stay) pos = orig_pos;
		
		temp_tok.type = string;
		return temp_tok;
	}

	/* if a unsigned number is found */
	if(isdigit(file[pos]))
	{
		while(isdigit(file[pos]))
			advance(pos, temp_tok.length);

		temp_tok.type = uint;

		if(match_ch(file[pos], '.') && isdigit(file[pos + 1]))
		{
			advance(pos, temp_tok.length);

			while(isdigit(file[pos]))
				advance(pos, temp_tok.length);
			
			if(isdigit(file[pos - 1])) temp_tok.type = unum;
		}
	
		/* CHECK THIS, SOME INVALID TYPES ARE ASSIGNED, EXAMPLE: 2.5E !!! */
		if(match_ch(file[pos], 'e') || match_ch(file[pos], 'E'))
		{
			temp_tok.type = unknown;
			
			advance(pos, temp_tok.length);

			if(match_ch(file[pos], '+') || match_ch(file[pos], '-'))
				advance(pos, temp_tok.length);

			while(isdigit(file[pos]))
				advance(pos, temp_tok.length);
			
			/* verify that the unsigned number is correctly formed */
			if(isdigit(file[pos - 1])) temp_tok.type = unum;
		}
	}

	/* if anything else is found */

	/* read until special_set symbol, or until whitespace */
	while(!match_set(file[pos], whitespace) && !match_set(file[pos], special_set))
	{
		advance(pos, temp_tok.length);
		temp_tok.type = unknown;
	}
	
	/* find types not found during scanning */
	
	/* used to find an identifier, PROBABLY SHOULD REWORK */
	{
		char found = 1;
		char *i = temp_tok.sym;
		unsigned long int j;
		
		if(!isalpha(*i))
			found = 0;
		
		for(j = 0; j < temp_tok.length; j++)
			if(!isalpha(i[j]) && !isdigit(i[j]))
				found = 0;
		
		if(found) temp_tok.type = identifier;
	}

	/* determine if a symbol is a word symbol */
	{
		char *word_symbols[] = {
			"and",    "end",      "nil",       "set",
			"array",  "file",     "not",       "then",
			"begin",  "for",      "of",        "to",
			"case",   "function", "or",        "type",
			"const",  "goto",     "packed",    "until",
			"div",    "if",       "procedure", "var",
			"do",     "in",       "program",   "while",
			"downto", "label",    "record",    "with",
			"else",   "mod",      "repeat"
		};
		
		unsigned long int i;
		for(i = 0; i < sizeof(word_symbols) / sizeof(word_symbols[0]); i++)
		{	
			/* find word symbol length */
			unsigned long int word_length;
			for(word_length = 0; word_symbols[i][word_length] != '\0'; word_length++);
		
			if(temp_tok.length == word_length)
			{
				char found = 1;
				
				/* test if every character is equivalent, if not, found = 0 */
				
				/* may replace with strcmp */
				
				unsigned long int j, k;
				for(j = 0, k = 0; j < word_length; j++, k++)
					if(word_symbols[i][j] != temp_tok.sym[k]) found = 0;
			
				if(found) temp_tok.type = word;
			}
		}
	}
	
	if(c == stay) pos = orig_pos;
	
	return temp_tok;
}

void prt_sym(token temp_tok)
{	
	unsigned long int i;

	for(i = 0; i < temp_tok.length; i++)
		printf("%c", temp_tok.sym[i]);
}

void prt_type(token temp_tok)
{
	switch(temp_tok.type)
	{
		case unknown: printf("unknown"); break;
		case special: printf("special"); break;
		case word: printf("word"); break;
		case identifier: printf("identifier"); break;
		case uint: printf("uint"); break;
		case unum: printf("unum"); break;
		case string: printf("string"); break;
		case EOI: printf("EOI"); break;
	}
}