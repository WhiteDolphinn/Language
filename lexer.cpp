#include "lexer.h"
#include <string.h>
#include <ctype.h>
#include "expression_reader.h"

#define POISON 394342

bool tokenizator(struct token* tokens, char* source_str, char** var_table)
{
   // struct token tokens[MAX_NUM_OF_TOKENS] = {};
    for(int i = 0; i < MAX_NUM_OF_TOKENS; i++)
    {
        tokens->type = POISON;
        tokens->value.int_val = POISON;
    }

    int current_var_num = 0;
    int current_token_num = 0;
    int index = 0;

    while(source_str[index] != '\0')
    {
        skip_spaces(source_str, &index);

        if(source_str[index] == '\0')
            break;
        if(current_token_num == MAX_NUM_OF_TOKENS)
            break;

        //printf("source_str[index] = %c\n", source_str[index]);
        #define DEFOP(FUNC, CODE, NAME)\
            if(is_this_word(source_str, &index, NAME))\
            {\
                tokens[current_token_num].type = OP;\
                tokens[current_token_num].value.int_val = CODE;\
                current_token_num++;\
                continue;\
            }\

        #define DEFFUNC(FUNC, CODE, NAME)\
            if(is_this_word(source_str, &index, NAME))\
            {\
                tokens[current_token_num].type = FUN;\
                tokens[current_token_num].value.int_val = CODE;\
                current_token_num++;\
                continue;\
            }\

        #define DEFLOGIC(FUNC, CODE, NAME)\
            if(is_this_word(source_str, &index, NAME))\
            {\
                tokens[current_token_num].type = LOGIC;\
                tokens[current_token_num].value.int_val = CODE;\
                current_token_num++;\
                continue;\
            }\

        #define DEFKEYWORD(FUNC, CODE, NAME)\
            if(is_this_word(source_str, &index, NAME))\
            {\
                tokens[current_token_num].type = KEYWORD;\
                tokens[current_token_num].value.int_val = CODE;\
                current_token_num++;\
                continue;\
            }\

        #define DEFBRACK(FUNC, CODE, NAME)\
            if(is_this_word(source_str, &index, NAME))\
            {\
                tokens[current_token_num].type = BRACK;\
                tokens[current_token_num].value.int_val = CODE;\
                current_token_num++;\
                continue;\
            }\

        #include "funcs.h"
        #undef DEFOP
        #undef DEFFUNC
        #undef DEFLOGIC
        #undef DEFKEYWORD
        #undef DEFBRACK

        printf("Symbol is %c (%d).\n", source_str[index], source_str[index]);

        if(isalpha(source_str[index]) || source_str[index] == '-')
        {
            char var_name[MAX_VAR_LENGTH] = {};
            /*sscanf(source_str + index, "%s ", var_name);
            index += (int)strlen(var_name);*/
            for(int i = 0; isalpha(source_str[index]); i++)
            {
                var_name[i] = source_str[index];
                index++;
            }

            bool is_created_var = false;
            for(int i = 0; i < MAX_NUM_OF_VARS; i++)
            {
                if(!strcmp((char*)(var_table) + MAX_VAR_LENGTH*i, var_name))
                {
                    tokens[current_token_num].type = VARIABLE;
                    tokens[current_token_num].value.int_val = i;
                    current_token_num++;
                    is_created_var = true;
                    break;
                }
            }

            if(is_created_var)
                continue;

            for(int i = 0; var_name[i] != '\0'; i++)
                *((char*)(var_table) + MAX_VAR_LENGTH*current_var_num + i) = var_name[i];

            tokens[current_token_num].type = VARIABLE;
            tokens[current_token_num].value.int_val = current_var_num;
            current_token_num++;
            current_var_num++;
            continue;
        }

        if(isalnum(source_str[index]) && !isalpha(source_str[index]))
        {
            double num = 0;
            sscanf(source_str + index, "%lf ", &num);
            while((isalnum(source_str[index]) && !isalpha(source_str[index])) || source_str[index] == '.')
                index++;

            tokens[current_token_num].type = NUMB;
            tokens[current_token_num].value.double_val = num;
            current_token_num++;
            continue;
        }

        printf("Error in lexer unknown symbol %c (%d). index = %d\n", source_str[index], source_str[index], index);
        return false;

        /*for(int i = 0; !isspace(source_str[index]); i++)
        {
            *((char*)(var_table) + MAX_VAR_LENGTH*current_var_num + i) = source_str[index];
            index++;
             printf("var_table[%d][%d] =\t %c\n", current_var_num, i, *((char*)(var_table) + MAX_VAR_LENGTH*current_var_num + i));
        }
        current_var_num++;*/
    }


    if(current_token_num == MAX_NUM_OF_TOKENS)
    {
        printf("tokens array overflow\n");
        return false;
    }

    return true;
    //return tokens;
}
