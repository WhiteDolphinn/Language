#include <ctype.h>
#include "expression_reader.h"
#include "text.h"

static int get_g(char* expr, Node** root);
static struct Node* get_n(char* expr, int* index);
static struct Node* get_e(char* expr, int* index);
static struct Node* get_t(char* expr, int* index);
static struct Node* get_pow_func(char* expr, int* index);
static struct Node* get_p(char* expr, int* index);
static struct Node* get_f(char* expr, int* index);

void skip_spaces(char* expr, int* index)
{
    while(isspace(expr[*index]))
        (*index)++;

    return;
}

int read_expession_rec_descent(FILE* source_file, Node** root)
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
}


static int get_g(char* expr, Node** root)
{
    int index = 0;
    *root  = get_e(expr, &index);

    if((*root)->type == SYNTAX_ERROR)
        return (int)(*root)->value;

    if(expr[index] != '\0')
    {
        printf("Syntax error in pos.%d. Symbol is %c\n", index, expr[index]);
        return SYNTAX_ERROR_IN_GET_G;
    }
    return 0;
}

static struct Node* get_n(char* expr, int* index)
{
    double value = 0;

    bool is_neg = expr[*index] == '-';
    if(is_neg)  (*index)++;

    if((expr[*index] > '9' || expr[*index] < '0'))
    {
        /*printf("Syntax error in pos.%d. Symbol is %c but expected number\n", *index, expr[*index]);
        return create_node(SYNTAX_ERROR, SYNTAX_ERROR_IN_GET_N);*/
        (*index)++;
        return create_node(VAR, expr[(*index) - 1]);
    }

    while(expr[*index] <= '9' && expr[*index] >= '0')
    {
        double value2 = expr[*index] - '0';
        (*index)++;
        value = 10 * value + value2;
    }

    if(expr[*index] == '.' || expr[*index] == ',')
    {
        (*index)++;
        double double_part = 0;
        double ten_pow = 10;

        while(expr[*index] <= '9' && expr[*index] >= '0')
        {
            double_part += (expr[*index] - '0') / ten_pow;
            (*index)++;
            ten_pow *= 10;
        }

        value += double_part;
    }

    if(!is_neg)
        return create_node(NUMBER, value);
    else
        return create_node(NUMBER, -value);
}

static struct Node* get_e(char* expr, int* index)
{
    struct Node* answer = get_t(expr, index);
    struct Node* cur_node = answer;

    while(expr[*index] == '+' || expr[*index] == '-')
    {
        int op = expr[*index];
        (*index)++;
        cur_node = create_node(op, op, answer);
        answer = cur_node;
        answer->right = get_t(expr, index);
    }
    return answer;
}

static struct Node* get_t(char* expr, int* index)
{
    struct Node* answer = get_pow_func(expr, index);
    struct Node* cur_node = answer;

    while(expr[*index] == '*' || expr[*index] == '/')
    {
        int op = expr[*index];
        (*index)++;
        cur_node = create_node(op, op, answer);
        answer = cur_node;
        answer->right = get_pow_func(expr, index);
    }
    return answer;
}

static struct Node* get_pow_func(char* expr, int* index)
{
    struct Node* answer = get_p(expr, index);
    struct Node* cur_node = answer;

    while(expr[*index] == '^')
    {
        int op = expr[*index];
        (*index)++;
        cur_node = create_node(op, op, answer);
        answer = cur_node;
        answer->right = get_p(expr, index);
    }
    return answer;
}

static struct Node* get_p(char* expr, int* index)
{
    if(expr[*index] == '(')
    {
        (*index)++;
        struct Node* answer = get_e(expr, index);

        if(expr[*index] != ')')
            printf("Syntax error in pos.%d. Symbol is %c but expected )\n", *index, expr[*index]);/////////////////////

        (*index)++;
        return answer;
    }

    return get_f(expr, index);
}

static struct Node* get_f(char* expr, int* index)
{
    struct Node* answer = nullptr;
    struct Node* func_argument = nullptr;

    char func[MAX_FUNC_LENGTH] = {};
    sscanf(expr + (*index), "%[^(]", func);
    /*while(expr[*index] != '(')  (*index)++;
    (*index)++;*/
    //printf("func = %s\n", func);

    #define DEFFUNC(SYMB, FUNC, PUSH, DIFF)         \
        if(!stricmp(func, #FUNC))                   \
        {                                           \
            while(expr[*index] != '(')  (*index)++; \
            func_argument = get_p(expr, index);     \
            answer = create_node(SYMB, SYMB);       \
            answer->left = func_argument;           \
            return answer;                          \
        }                                           \

    #include "def_funcs.h"

    #undef DEFFUNC

    return get_n(expr, index);

}
