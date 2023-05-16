#ifndef LEXER_H
#define LEXER_H

struct token{
    int value;
    int type;
};

enum token_type{
    OP = 1,
    FUN = 2,
    LOGIC = 3,
    KEYWORD = 4,
    BRACK = 5,
    ARGS = 6,
    INFO = 7,
    VARIABLE = 8,
    NUMB = 9,
    FUNC = 10,
};

void tokenizator(struct token* tokens, char* source_str, char** var_table);

#endif
