#ifndef LEXER_H
#define LEXER_H

#define MAX_NUM_OF_TOKENS 1024

#define MAX_VAR_LENGTH 20
#define MAX_NUM_OF_VARS 15
#define MAX_FUNC_LENGTH 50
#define MAX_NUM_OF_FUNCS 15 //not more than MAX_NUM_OF_VARS

union type_val{
    int int_val;
    double double_val;
};

struct token{
    type_val value;
    int type;
};

enum token_types{
    OP = 1,
    FUN = 2,
    LOGIC = 3,
    KEYWORD = 4,
    BRACK = 5,
    ARGS = 6,
    INFO = 7,
    VARIABLE = 8,
    NUMB = 9,
};

bool tokenizator(struct token* tokens, char* source_str, char** var_table, char** func_table);

#endif
