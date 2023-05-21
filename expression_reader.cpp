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

    if(op != 0)
    {
        (*index)++;
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

    while(TOKEN_INT(OP, ADDF) || TOKEN_INT(OP, SUBF))
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

    while(TOKEN_INT(OP, MULF) || TOKEN_INT(OP, DIVF))
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
        if(tokens[(*index)+1].type == BRACK && tokens[(*index)+1].value.int_val == OCB)
        {
            struct Node* name = get_id(tokens, index);

            if(!TOKEN_INT(BRACK, OCB))
            {
                printf("Syntax error in pos.%d. Func: get_p.\tExpected (\n", *index);
                return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_P, name);
            }
            (*index)++;

            if(TOKEN_INT(BRACK, CCB))
                return name;

            struct Node* arg = get_e(tokens, index);
            //printf("zhopa kozla\n");

            while(TOKEN_INT(OP, COM))
            {
               // printf("zhopa kozla\n");
                (*index)++;
                struct Node* copy_arg = copy_node(arg);
                delete_tree_without_root(arg);
                arg->type = OP;
                arg->value = COM;
                arg->left = copy_arg;
                arg->right = get_e(tokens, index);
            }

            if(!TOKEN_INT(BRACK, CCB))
            {
                printf("Syntax error in pos.%d. Func: get_p\tExpected )\n", *index);
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
        printf("Syntax error in pos.%d. Func: get_a Expected variable\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_A);
    }

    if(TOKEN_INT(OP, ASS))
        (*index)++;
    else
    {
        printf("Syntax error in pos.%d. Func: get_aa Expected =\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_A);
    }

   /* double val = 0;
    if(tokens[*index].type == NUMB)
    {
        val = tokens[*index].value.double_val;
        (*index)++;
    }
    else
    {
        if(tokens[*index].type == FUN || tokens[*index].type == VARIABLE)
        {

        }
        else
        {
            printf("Syntax error in pos.%d. Expected variable\n", *index);
            return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_A);
        }
    }

    struct Node* var_node = create_node(VARIABLE, var_num);
    struct Node* val_node = create_node(NUMB, val);

    if(is_var)
    {

        struct Node* answer =  create_node(KEYWORD, VAR, var_node, val_node);
        //tree_print(answer);
        return answer;
    }
    else
        return create_node(OP, ASS, var_node, val_node);*/

    struct Node* var_node = create_node(VARIABLE, var_num);

    switch(tokens[*index].type)
    {
        case NUMB:
        {
            double val = tokens[*index].value.double_val;
            struct Node* val_node = create_node(NUMB, val);
            (*index)++;

            if(is_var)
                return create_node(KEYWORD, VAR, var_node, val_node);
            else
                return create_node(OP, ASS, var_node, val_node);
        }
        case VARIABLE:
        {
            (*index)++;
            struct Node* var2_node = create_node(VARIABLE, tokens[*index].value.int_val);
            (*index)++;
            return create_node(OP, ASS, var_node, var2_node);
        }

        case FUN:
        {
            //(*index)++;
            struct Node* func = create_node(FUN, tokens[*index].value.int_val);
            //printf("index = %d\n", *index);
            (*index)++;

            if(TOKEN_INT(BRACK, OCB))
                (*index)++;
            else
            {
                printf("Syntax error in pos.%d. Func: get_a Expected (\n", *index);
                return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_A);
            }

            if(tokens[*index].type == VARIABLE || tokens[*index].type == NUMB || tokens[*index].type == FUN)
            {
                if(tokens[*index].type == NUMB)
                    func->left = create_node(tokens[*index].type, tokens[*index].value.double_val);
                else
                    func->left = create_node(tokens[*index].type, tokens[*index].value.int_val);

                (*index)++;

                while(TOKEN_INT(OP, COM))
                {
                    (*index)++;
                    struct Node* copy_cur_node = copy_node(func->left);
                    delete_tree_without_root(func->left);
                    //func->left = create_node(OP, COM, copy_cur_node);
                    func->left->left = copy_cur_node;

                    func->left->type = OP;
                    func->left->value = COM;

                    printf("index = %d\n", *index);
                    func->left->right = get_e(tokens, index);
                    //(*index)++;
                }
            }

            if(TOKEN_INT(BRACK, CCB))
                (*index)++;
            else
            {
                printf("Syntax error in pos.%d. Func: get_a  Expected )\n", *index);
                return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_A);
            }

            return create_node(OP, ASS, var_node, func);
        }

        default: break;
    }

    printf("Syntax error in pos.%d. Func: get_a Expected variable|func or number\n", *index);
    return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_A);
}

static struct Node* get_if(struct token* tokens, int* index)
{
    if(!TOKEN_INT(LOGIC, IF))
    {
        printf("Syntax error in pos.%d. Expected if\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_IF);
    }
    (*index)++;

    if(!TOKEN_INT(BRACK, OCB))
    {
        printf("Syntax error in pos.%d. Func: get_if Expected (\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_IF);
    }
    (*index)++;

    //printf("index = %d\n", *index);
    struct Node* L = get_l(tokens, index);
   // printf("index = %d\n", *index);

    if(!TOKEN_INT(BRACK, CCB))
    {
        printf("Syntax error in pos.%d. Func: get_if Expected )\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_IF, L);
    }
    (*index)++;

    struct Node* comp = get_comp(tokens, index);

    return create_node(LOGIC, IF, L, comp);
}

static struct Node* get_op(struct token* tokens, int* index)
{
    /*if(expr[*index] == 'i' && expr[*(index+1)] == 'f')
        get_if(expr, index);*/
    struct Node* op = nullptr;
    if(TOKEN_INT(LOGIC, IF))
        op = get_if(tokens, index);

    if(op == nullptr && (TOKEN_INT(KEYWORD, DEC) || TOKEN_INT(FUN, PRINTF) || TOKEN_INT(FUN, SCANF)))
        op = get_func(tokens, index);

    if(op == nullptr && TOKEN_INT(BRACK, OSB))
        op =  get_comp(tokens, index);

    if(op == nullptr)
        op = get_a(tokens, index);

    if(!TOKEN_INT(KEYWORD, END))
    {
        printf("Syntax error in pos.%d. Func:get_op Expected $\n", *index);
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
        (*index)++;
        struct Node* func = create_node(KEYWORD, DEC);
        func->left = create_node(FUN, tokens[*index].value.int_val);
        (*index)++;

        if(!TOKEN_INT(BRACK, OCB))
        {
            printf("Syntax error in pos.%d. Func: get_func. Expected (\n", *index);
            return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_FUNC, func);
        }
        (*index)++;

        if(tokens[*index].type == NUMB || tokens[*index].type == VARIABLE)
        {
            func->left->left = create_node(tokens[*index].type, tokens[*index].value.int_val);
            (*index)++;

            while(TOKEN_INT(OP, COM))
            {
                (*index)++;
                struct Node* args = copy_node(func->left->left);
                delete_tree(func->left->left);

                struct Node* new_arg = create_node(tokens[*index].type, tokens[*index].value.int_val);
                (*index)++;
                func->left->left = create_node(OP, COM, args, new_arg);
                //func->left->type = OP;
            }
        }

        if(!TOKEN_INT(BRACK, CCB))
        {
            printf("Syntax error in pos.%d. Func: get_func. Expected )\n", *index);
            return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_FUNC, func);
        }
        (*index)++;

        if(!TOKEN_INT(BRACK, OFB))
        {
            printf("Syntax error in pos.%d. Func: get_func. Expected {\n", *index);
            return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_FUNC, func);
        }
        (*index)++;

        while(!TOKEN_INT(BRACK, CFB))
        {
            //static struct Node* current_op = func->right;
        //static struct Node* current_op = func->right;
        struct Node* copy_cur_op = copy_node(func->right);
        if(func->right != nullptr)
            delete_tree_without_root(func->right);
        else
        {
            func->right = get_op(tokens, index);
            continue;
        }

        func->right->left = copy_cur_op;
        func->right->type = OP;
        func->right->value = AND;

        //func->right->right = get_op(tokens, index);


            //tree_print(func->right);

            if(!TOKEN_INT(KEYWORD, RETURN))
                func->right->right = get_op(tokens, index);
            else
            {
                func->right->right = create_node(KEYWORD, RETURN);
                (*index)++;
                func->right->right->left = get_e(tokens, index);
                if(TOKEN_INT(KEYWORD, END))
                    (*index)++;
                break;
            }
        }

        if(!TOKEN_INT(BRACK, CFB))
        {
            printf("Syntax error in pos.%d. Func: get_func. Expected }\n", *index);
            return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_FUNC, func);
        }
        (*index)++;

        return func;
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
    op = create_node(KEYWORD, MAI);
    op->left = get_op(tokens, index);

    while(!TOKEN_INT(BRACK, CFB))
    {
        static struct Node* current_op = op->left;
        struct Node* copy_cur_op = copy_node(current_op);
        delete_tree_without_root(current_op);
        current_op->left = copy_cur_op;
        current_op->type = OP;
        current_op->value = AND;

        current_op->right = get_op(tokens, index);
    }

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
        printf("Syntax error in pos.%d. Expected (\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_SCANF);
    }
    (*index)++;

    //struct Node* id = get_id(tokens, index);
    if(tokens[*index].type != VARIABLE)
    {
        printf("Syntax error in pos.%d. Expected var/val\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_SCANF);
    }
    struct Node* id = create_node(tokens[*index].type, tokens[*index].value.int_val);
    (*index)++;

    if(!TOKEN_INT(BRACK, CCB))
    {
        printf("Syntax error in pos.%d. Expected )\n", *index);
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

    //struct Node* id = get_id(tokens, index);
    if(tokens[*index].type != VARIABLE)
    {
        printf("Syntax error in pos.%d. Expected var/val\n", *index);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_PRINTF);
    }
    struct Node* id = create_node(tokens[*index].type, tokens[*index].value.int_val);
    (*index)++;

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
