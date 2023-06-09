#include "tree_soft.h"
//#include "diff.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "log.h"
#include "dotter.h"
#include "lexer.h"

#define POISON 3472394

static void node_print(struct Node* node);

struct Node* create_node(int type, double value, struct Node* left, struct Node* right)
{
    struct Node* node = (struct Node*)calloc(1, sizeof(struct Node));

    if(node == nullptr)
    {
        printf("node is nullptr\n");
        return nullptr;
    }

    node->type = type;
    node->value = value;
    node->left = left;
    node->right = right;

    return node;
}

void push_node(struct Node* node, int type, double value)
{
    node->type = type;
    node->value = value;
}

void delete_tree(struct Node* node)
{
    if(node == nullptr)
        return;

    delete_tree(node->left);
    delete_tree(node->right);

    free(node);
}

void delete_tree_without_root(struct Node* node)
{
    delete_tree(node->left);
    delete_tree(node->right);
    node->left = nullptr;
    node->right = nullptr;
}

void tree_print(struct Node* tree)
{
    if(tree == nullptr)
        return;

    graph_start();
    graph_add_root(tree);

    node_print(tree);

    graph_end();
}

static void node_print(struct Node* node)
{
    if(node == nullptr)
        return;

    node_print(node->left);
    node_print(node->right);

   /* if(node->type == NUMB)
        graph_add_dot(node, node->value, node->type, node->left, node->right, "#FFD0D0");
    else if(node->type == VARIABLE)
        graph_add_dot(node, node->value, node->type, node->left, node->right, "#D0D0FF");
    else if(is_op_or_func_or_logic_or_keyword(node->type))
        graph_add_dot(node, node->value, node->type, node->left, node->right, "#D0FFD0");
    else
        graph_add_dot(node, node->value, node->type, node->left, node->right, "#FF0000");*/

    switch(node->type)
    {
        case OP:
            graph_add_dot(node, node->value, node->type, node->left, node->right, "#D0FFD0");
        break;

        case FUN:
            graph_add_dot(node, node->value, node->type, node->left, node->right, "#5c3c92");
        break;

        case LOGIC:
            graph_add_dot(node, node->value, node->type, node->left, node->right, "#077b8a");
        break;

        case KEYWORD:
            graph_add_dot(node, node->value, node->type, node->left, node->right, "#d72631");
        break;

        case BRACK:
            graph_add_dot(node, node->value, node->type, node->left, node->right, "#a2d5c6");
        break;

        case ARGS:
            graph_add_dot(node, node->value, node->type, node->left, node->right, "#ecc19c");
        break;

        case INFO:
            graph_add_dot(node, node->value, node->type, node->left, node->right, "#1e847f");
        break;

        case VARIABLE:
            graph_add_dot(node, node->value, node->type, node->left, node->right, "#D0D0FF");
        break;

        case NUMB:
            graph_add_dot(node, node->value, node->type, node->left, node->right, "#FFD0D0");
        break;

/*        case FUNC:
            graph_add_dot(node, node->value, node->type, node->left, node->right, "#316879");
        break;*/

        default:
            graph_add_dot(node, node->value, node->type, node->left, node->right, "#FF0000");
        break;
    }

    if(node->left != nullptr)
        graph_add_arrow(node, node->left, "#0000FF");

    if(node->right != nullptr)
        graph_add_arrow(node, node->right, "#FF0000");
}

void tree_print_inorder(FILE* file, struct Node* tree)
{
    if(tree == nullptr)
        return;

    if(tree->type == NUMBER || tree->type == VAR || tree->type == ADDF || tree->type == SUBF || tree->type == MULF || tree->type == DIVF || tree->type == POWF)
    {
        fprintf(file, "{ ");
        tree_print_inorder(file, tree->left);

        if(tree->type == NUMBER)
            fprintf(file, "%g ", tree->value);
        else if(tree->type == VAR)
            fprintf(file, "%c ", (char)tree->value);
        else
        {
            switch((int)tree->value)
            {
                case ADDF:   fprintf(file, "+ "); break;
                case SUBF:   fprintf(file, "- "); break;
                case MULF:   fprintf(file, "\\cdot "); break;
                case DIVF:   fprintf(file, "/ "); break;
                case POWF:   fprintf(file, "^ "); break;
                default:  fprintf(file, "bebra\n"); break;
            }
        }

        tree_print_inorder(file, tree->right);
        fprintf(file, "} ");
    }
    else
    {
        switch(tree->type)
        {
            case LN: fprintf(file, "ln(");   break;
            case SIN: fprintf(file, "sin("); break;
            case COS: fprintf(file,"cos("); break;
            case TAN: fprintf(file, "tan("); break;
            case COT: fprintf(file, "cot("); break;
            default:    break;
        }

        tree_print_inorder(file, tree->left);
        fprintf(file, ")");
    }
}

struct Node* copy_node(struct Node* node)
{
    if(node == nullptr)
        return nullptr;

    return create_node(node->type, node->value, copy_node(node->left), copy_node(node->right));
}

void optimizate_tree(struct Node* node)
{
    if(node->left == nullptr)
        return;

    if(is_number_tree(node))
    {
        double num = eval(node);
        delete_tree_without_root(node);
        push_node(node, NUMBER, num);
    }

    if(node->right != nullptr)
    {
        if(equal_double(node->left->value, 0) && equal_double(node->right->value, 0))
        {
            delete_tree_without_root(node);
            push_node(node, NUMBER, 0);
            return;
        }

        if((node->type == ADDF || node->type == SUBF) && equal_double(node->right->value, 0))
        {
            delete_tree(node->right);
            merge_nodes(node, node->left);
            return;
        }

        if(node->type == ADDF && equal_double(node->left->value, 0))
        {
            delete_tree(node->left);
            merge_nodes(node, node->right);
            return;
        }

        if(node->type == MULF && (equal_double(node->left->value, 0) || equal_double(node->right->value, 0)))
        {
            delete_tree_without_root(node);
            push_node(node, NUMBER, 0);
            return;
        }

        if((node->type == MULF || node->type == DIVF || node->type == POWF) && equal_double(node->right->value, 1))
        {
            delete_tree(node->right);
            merge_nodes(node, node->left);
            return;
        }

        if(node->type == MULF && equal_double(node->left->value, 1))
        {
            delete_tree(node->left);
            merge_nodes(node, node->right);
            return;
        }
        if(node->type == POWF && equal_double(node->right->value, 0))
        {
            delete_tree_without_root(node);
            push_node(node, NUMBER, 1);
            return;
        }
        if(node->type == POWF && equal_double(node->right->value, 1))
        {
            delete_tree(node->right);
            merge_nodes(node, node->left);
            return;
        }
    }
    else
    {
        if(node->type == LN && node->left->type == POWF)
        {
            struct Node* stepen = copy_node(node->left->right);
            struct Node* osnov = copy_node(node->left->left);
            delete_tree_without_root(node);
            push_node(node, MULF, MULF);

            node->left = create_node(257, 257);
            node->right = create_node(LN, LN);
            node->right->left = create_node(257, 257);

            merge_nodes(node->left, stepen);
            merge_nodes(node->right->left, osnov);
            return;
        }
    }
    //printf("Я ёбанная параша(функция оптимизации) передаю %p и %p\n", &((*node)->left), &((*node)->right));

    if(node->left != nullptr)
        optimizate_tree(node->left);

    if(node->right != nullptr)
        optimizate_tree(node->right);
}

int equal_double(double a, double b)
{
    return fabs(a - b) < 1e-7;
}

bool is_number_tree(struct Node* node)
{
    if(node == nullptr)
        return false;

    switch(node->type)
    {
        case NUMBER: return true;

        case VAR: return false;

        case ADDF: case SUBF: case MULF: case DIVF: case POWF:
            return is_number_tree(node->left) && is_number_tree(node->right);

        case LN: case SIN: case COS: case TAN: case COT:
            return is_number_tree(node->left);

        default: return false;

    }
    return false;
}

void merge_nodes(struct Node* in_node, struct Node* out_node)
{
    in_node->type = out_node->type;
    in_node->value = out_node->value;
    in_node->left = out_node->left;
    in_node->right = out_node->right;

    free(out_node);
}

bool is_op_or_func_or_logic_or_keyword(int type)
{
    #define DEFOP(FUNC, CODE, NAME)             \
        if(CODE == type)                        \
            return true;                        \

    #define DEFFUNC(FUNC, CODE, NAME)           \
        if(CODE == type)                        \
            return true;                        \

    #define DEFLOGIC(FUNC, CODE, NAME)          \
        if(CODE == type)                        \
            return true;                        \

    #define DEFKEYWORD(FUNC, CODE, NAME)        \
        if(CODE == type)                        \
            return true;                        \

    #define DEFBRACK(FUNC, CODE, NAME)          \
        if(CODE == type)                        \
            return true;                        \

    #include "funcs.h"
    #undef DEFOP
    #undef DEFFUNC
    #undef DEFLOGIC
    #undef DEFKEYWORD
    #undef DEFBRACK

    return false;
}

double eval(struct Node* node)
{
    if(!is_number_tree(node))
        return POISON;

    if(node->type == NUMBER)
        return node->value;

    switch(node->type)
    {
        case ADDF:
            return eval(node->left) + eval(node->right);
        case SUBF:
            return eval(node->left) - eval(node->right);
        case MULF:
            return eval(node->left) * eval(node->right);
        case DIVF:
        {
            double right_val = eval(node->right);
            if(equal_double(right_val, 0))
            {
                printf("division by zero!\n");
                return POISON;
            }
            return eval(node->left) / eval(node->right);
        }
        case POWF:
            return pow(eval(node->left), eval(node->right));
        case LN:
        {
            double val = eval(node->left);
            if(equal_double(val, 0))
            {
                printf("ln(0) = inf\n");
                return POISON;
            }
            return log(val);
        }
        case SIN:
            return sin(eval(node->left));
        case COS:
            return cos(eval(node->left));
        case TAN:
        {
            double val = eval(node->left);
            if(equal_double(cos(val), 0))
            {
                printf("tan(pi/2 + 2 pi k = inf\n)");
                return POISON;
            }
            return tan(val);
        }
        case COT:
        {
            double val = eval(node->left);
            if(equal_double(sin(val), 0))
            {
                printf("cot2 pi k = inf\n");
                return POISON;
            }
            return 1/tan(val);
        }
        default:
            return POISON;
    }
    return POISON;
}

