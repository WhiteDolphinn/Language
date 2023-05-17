#include <ctype.h>
#include "expression_reader.h"
#include "text.h"
#include "lexer.h"

#define TOKEN_INT(TYPE, VAL)                                        \
(tokens[*index].type == TYPE && tokens[*index].value.int_val == VAL)\

#define TOKEN_DOUBLE(TYPE, VAL)                                        \
(tokens[*index].type == TYPE && tokens[*index].value.double_val == VAL)\

static int get_g(struct token* tokens, Node** root);
static struct Node* get_l(struct token* tokens, int* index);
static struct Node* get_e(struct token* tokens, int* index);
static struct Node* get_t(struct token* tokens, int* index);
static struct Node* get_p(struct token* tokens, int* index);
static struct Node* get_n(struct token* tokens, int* index);
static struct Node* get_id(struct token* tokens, int* index);
static struct Node* get_a(struct token* tokens, int* index);
static struct Node* get_if(struct token* tokens, int* index);
static struct Node* get_op(struct token* tokens, int* index);
static struct Node* get_comp(struct token* tokens, int* index);
static struct Node* get_func(struct token* tokens, int* index);
static struct Node* get_main(struct token* tokens, int* index);
static struct Node* get_scanf(struct token* tokens, int* index);
static struct Node* get_printf(struct token* tokens, int* index);

void skip_spaces(char* expr, int* index)
{
    while(isspace(expr[*index]))
        (*index)++;

    return;
}

/*int read_expession_rec_descent(FILE* source_file, Node** root)
{
    char expession_with_spaces[MAX_STR_LENGTH] = {};
    char expession_without_spaces[MAX_STR_LENGTH] = {};

    if(root == nullptr)
    {
        printf("root == nullptr\n");
        return ERROR_ROOT_POINTER;
    }

    if(!fscanf(source_file, " %[^\n]", expession_with_spaces))
    {
        printf("I can't read expession\n");
        return ERROR_READ_EXPRESSION;
    }
    if(expession_with_spaces[MAX_STR_LENGTH - 1] != '\0')
    {
        printf("Переполение буфера expression\n");
        return ERROR_BUFFER_OVERFLOW;
    }

    int cur_exp_pos = 0;
    for(int i = 0; expession_with_spaces[i] != '\0'; i++)
        if(!isspace(expession_with_spaces[i]))
            expession_without_spaces[cur_exp_pos++] = expession_with_spaces[i];

    return get_g(expession_without_spaces, root);
}*/

int convert_tokens_to_ast(struct token* tokens, Node** root)
{
    if(tokens == nullptr)
    {
        printf("tokens == nullptr\n");
        return ERROR_TOKENS_ARR;
    }
    if(root == nullptr)
    {
        printf("root == nullptr\n");
        return ERROR_ROOT_POINTER;
    }

    return get_g(tokens, root);
}


static int get_g(struct token* tokens, Node** root)
{
    int index = 0;
    *root = get_main(tokens, &index);

    if((*root)->type == SYNTAX_ERROR)
        return (int)(*root)->value;

    if(tokens[index].type != KEYWORD || tokens[index].value.int_val != END)
    {
        printf("Syntax error in pos.%d. Symbol is %c but expected $\n", index, tokens[index].value.int_val);
        return SYNTAX_ERROR_IN_GET_G;
    }
    return 0;
}

static struct Node* get_l(struct token* tokens, int* index)
{
    struct Node* answer = get_e(tokens, index);
    struct Node* cur_node = answer;

    int op = 0;
    if(TOKEN_INT(OP, MOR))  op = MOR;
    if(TOKEN_INT(OP, MOE))  op = MOE;
    if(TOKEN_INT(OP, EQU))  op = EQU;
    if(TOKEN_INT(OP, NEQ))    op = NEQ;

    (*index)++;

    if(op != 0)
    {
        //int op = expr[*index];
        //(*index)++;
        cur_node = create_node(OP, op, answer);
        answer = cur_node;
        answer->right = get_e(tokens, index);
    }

    return answer;
}

static struct Node* get_e(struct token* tokens, int* index)
{
    struct Node* answer = get_t(tokens, index);
    struct Node* cur_node = answer;

    while(TOKEN_INT(OP, ADD) || TOKEN_INT(OP, SUB))
    {
        int op = tokens[*index].value.int_val;
        (*index)++;
        cur_node = create_node(OP, op, answer);
        answer = cur_node;
        answer->right = get_t(tokens, index);
    }
    return answer;
}

static struct Node* get_t(struct token* tokens, int* index)
{
    struct Node* answer = get_p(tokens, index);
    struct Node* cur_node = answer;

    while(TOKEN_INT(OP, MUL) || TOKEN_INT(OP, DIV))
    {
        int op = tokens[*index].value.int_val;
        (*index)++;
        cur_node = create_node(OP, op, answer);
        answer = cur_node;
        answer->right = get_p(tokens, index);
    }
    return answer;
}

static struct Node* get_p(struct token* tokens, int* index)
{
    if(TOKEN_INT(BRACK, OCB))
    {
        (*index)++;
        struct Node* answer = get_e(tokens, index);

        if(!TOKEN_INT(BRACK, CCB))
        {
            printf("Syntax error in pos.%d. Expected )\n", *index);
            return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_P, answer);
        }

        (*index)++;
        return answer;
    }

    //пока без Id'('E')'

    if(tokens[*index].type == VARIABLE || tokens[*index].type == FUN)
    {
        if(tokens[(*index)+1].type == BRACK && tokens[(*index)+1].value.int_val)
        {
            struct Node* name = get_id(tokens, index);

            if(!TOKEN_INT(BRACK, OCB))
            {
                printf("Syntax error in pos.%d. Expected (\n", *index);
                return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_P, name);
            }
            (*index)++;

            struct Node* arg = get_e(tokens, index);

            if(!TOKEN_INT(BRACK, CCB))
            {
                printf("Syntax error in pos.%d. Expected )\n", *index);
                return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_P, name);
            }
            (*index)++;

            name->left = arg;
            return name;
        }
        else
            return get_id(tokens, index);
    }


    return get_n(tokens, index);
}

static struct Node* get_n(struct token* tokens, int* index)
{
    if(tokens[*index].type != NUMB)
    {
        printf("Syntax error in pos.%d. Expected number.\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_N);
    }

    (*index)++;
    return create_node(NUMB, tokens[(*index)-1].value.double_val);
}

static struct Node* get_id(struct token* tokens, int* index)
{
    if(tokens[*index].type!= VARIABLE && tokens[*index].type != FUN && tokens[*index].type != KEYWORD)
    {
        printf("Syntax error in pos.%d. Expected FUN/VAR/KEYWORD\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_ID);
    }

    (*index)++;

    return create_node(tokens[(*index)-1].type, tokens[(*index)-1].value.int_val);
}

static struct Node* get_a(struct token* tokens, int* index)
{
    bool is_var = false;

    if(TOKEN_INT(KEYWORD, VAR))
    {
        is_var = true;
        (*index)++;
    }

    int var_num = 0;
    if(tokens[*index].type == VARIABLE)
    {
        var_num = tokens[*index].value.int_val;
        (*index)++;
    }
    else
    {
        printf("Syntax error in pos.%d. Expected variable\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_A);
    }

    if(TOKEN_INT(OP, ASS))
        (*index)++;
    else
    {
        printf("Syntax error in pos.%d. Expected =\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_A);
    }

    double val = 0;
    if(tokens[*index].type == NUMB)
    {
        val = tokens[*index].value.double_val;
        (*index)++;
    }
    else
    {
        printf("Syntax error in pos.%d. Expected variable\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_A);
    }

    struct Node* var_node = create_node(NUMB, var_num);
    struct Node* val_node = create_node(NUMB, val);

    if(is_var)
        return create_node(OP, VAR, var_node, val_node);
    else
        return create_node(OP, ASS, var_node, val_node);
}

static struct Node* get_if(struct token* tokens, int* index)
{

    if(!TOKEN_INT(BRACK, OCB))
    {
        printf("Syntax error in pos.%d. Expected (", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_IF);
    }
    (*index)++;

    struct Node* L = get_l(tokens, index);

    if(!TOKEN_INT(BRACK, CCB))
    {
        printf("Syntax error in pos.%d. Expected )", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_IF, L);
    }
    (*index)++;

    struct Node* comp = get_comp(tokens, index);

    return create_node(IF, IF, L, comp);
}

static struct Node* get_op(struct token* tokens, int* index)
{
    /*if(expr[*index] == 'i' && expr[*(index+1)] == 'f')
        get_if(expr, index);*/
    struct Node* op = nullptr;
    if(TOKEN_INT(LOGIC, IF))
        op = get_if(tokens, index);

    if(op == nullptr && TOKEN_INT(KEYWORD, DEC))
        op = get_func(tokens, index);

    if(op == nullptr && TOKEN_INT(BRACK, OSB))
        op =  get_comp(tokens, index);

    if(op == nullptr)
        op = get_a(tokens, index);

    if(!TOKEN_INT(KEYWORD, END))
    {
        printf("Syntax error in pos.%d. Expected $\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_OP, op);
    }

    (*index)++;
    return op;
}

static struct Node* get_comp(struct token* tokens, int* index)
{
    if(!TOKEN_INT(BRACK, OSB))
    {
        printf("Syntax error in pos.%d. Expected [", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_COMP);
    }
    (*index)++;

    struct Node* answer = get_op(tokens, index);
    struct Node* cur_node = answer;

    while(TOKEN_INT(OP, AND))
    {
        (*index)++;
        cur_node = create_node(OP, AND, answer);
        answer = cur_node;
        answer->right = get_op(tokens, index);
    }

    if(!TOKEN_INT(BRACK, CSB))
    {
        printf("Syntax error in pos.%d. Expected ]\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_COMP);
    }
    (*index)++;

    return answer;
}

static struct Node* get_func(struct token* tokens, int* index)
{
    if(TOKEN_INT(KEYWORD, DEC))
    {
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_FUNC);

    }

    if(TOKEN_INT(FUN, SCANF))
        return get_scanf(tokens, index);

    if(TOKEN_INT(FUN, PRINTF))
        return get_printf(tokens, index);

    printf("Syntax error in pos.%d. Expected func\n", *index);
    return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_FUNC);
}

static struct Node* get_main(struct token* tokens, int* index)
{
    if(TOKEN_INT(KEYWORD, MAI))
        (*index)++;
    else
    {
        printf("Error in %d. Expected main().\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_MAIN);
    }

    if(TOKEN_INT(BRACK, OFB))
        (*index)++;
    else
    {
        printf("Error in %d. Expected {.\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_MAIN);
    }

    struct Node* op = nullptr;
    do
    {
        op = get_op(tokens, index);
        struct Node* old_op = op;
        op = create_node(OP, AND, old_op);
    }while(TOKEN_INT(KEYWORD, END));

    op->type = KEYWORD;
    op->value = MAI;

    if(TOKEN_INT(BRACK, CFB))
        (*index)++;
    else
    {
        printf("Error in %d. Expected }.\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_MAIN, op);
    }

    return op;
}

static struct Node* get_scanf(struct token* tokens, int* index)
{
    if(!TOKEN_INT(FUN, SCANF))
    {
        printf("Syntax error in pos.%d. Expected scanf", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_SCANF);
    }
    (*index)++;

    if(!TOKEN_INT(BRACK, OCB))
    {
        printf("Syntax error in pos.%d. Expected (", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_SCANF);
    }
    (*index)++;

    struct Node* id = get_id(tokens, index);

    if(!TOKEN_INT(BRACK, CCB))
    {
        printf("Syntax error in pos.%d. Expected )", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_SCANF);
    }
    (*index)++;

    return create_node(FUN, SCANF, id);
}

static struct Node* get_printf(struct token* tokens, int* index)
{
    if(!TOKEN_INT(FUN, PRINTF))
    {
        printf("Syntax error in pos.%d. Expected printf", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_PRINTF);
    }
    (*index)++;

    if(!TOKEN_INT(BRACK, OCB))
    {
        printf("Syntax error in pos.%d. Expected (", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_PRINTF);
    }
    (*index)++;

    struct Node* id = get_id(tokens, index);

    if(!TOKEN_INT(BRACK, CCB))
    {
        printf("Syntax error in pos.%d. Expected )", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_PRINTF);
    }
    (*index)++;

    return create_node(FUN, PRINTF, id);
}

bool is_this_word(char* expr, int* index, const char* word)
{
    int i = 0;
    for(; word[i] != '\0'; i++)
        if(word[i] != expr[(*index) + i])
            return false;

    (*index) += i;
    return true;
}
