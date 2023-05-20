#include "backend.h"
#include "tree_soft.h"
#include "lexer.h"
#include "soft_cpu.h"
#include <math.h>

int emit_prog(struct Node* root, FILE* assembler_file)
{
    if(root == nullptr)
        return ERROR_NULL_ROOT;

    struct Node* cur_node = root;
    //printf("%d %lf\n", cur_node->type, cur_node->value);

    if(!NODE(KEYWORD, MAI))
        return ERROR_MAIN;

    printf("%d %lf\n", cur_node->left->type, cur_node->left->value);

    if(NODE_LEFT(OP, AND))
        return emit_tree(cur_node->left, assembler_file);

    return emit_node(cur_node->left, assembler_file);
}

int emit_tree(struct Node* root, FILE* assembler_file)
{
    static int error = 0;
    if(error != 0)
        return error;

    if(root == nullptr)
        return ERROR_NULL_NODE_PTR;

    struct Node* cur_node = root;

    if(cur_node->left == nullptr)
        return ERROR_NULL_LEFT_NODE_PTR;

    if(cur_node->right == nullptr)
        return ERROR_NULL_RIGHT_NODE_PTR;

    if(NODE(OP, AND))
        error = emit_tree(cur_node->left, assembler_file);
    else if(cur_node->left->type != VARIABLE && cur_node->left->type != NUMB)
        error = emit_node(cur_node->left, assembler_file);

    error = emit_node(cur_node->right, assembler_file);
    return error;
}

int emit_node(struct Node* root, FILE* assembler_file)
{
    static bool is_declared_vars[MAX_NUM_OF_VARS] = {0};
    static bool is_declared_funcs[MAX_NUM_OF_FUNCS] = {0};
    struct Node* cur_node = root;
    int error = 0;


    if(root == nullptr)
        return ERROR_NULL_NODE_PTR;

    switch(root->type)
    {
        case KEYWORD:
        {
            switch((int)root->value)
            {
                case VAR:
                {

                    if(root->left->type != VARIABLE)
                        return ERROR_IN_ASS;

                    if(is_declared_vars[(int)root->left->value])
                        return ERROR_REDEFINATION_VAR;

                    is_declared_vars[(int)root->left->value] = true;

                    /*error = emit_node(root->right, assembler_file);

                    fprintf(assembler_file, "pop [%d]\n", (int)root->left->value);
                    break;*/

                    error = emit_node(root->right, assembler_file);
                    fprintf(assembler_file, "pop [%d]\n", (int)root->left->value);
                    break;
                }
                case DEC:
                {
                    if(is_declared_funcs[(int)root->left->value])
                        return ERROR_REDEFINATION_FUNC;

                    while(NODE_LEFT(OP, COM))
                    {
                        if(cur_node->right->type == VARIABLE)
                            fprintf(assembler_file, "pop [%d]\n", (int)cur_node->right->value);

                    }

                    cur_node = root;

                    if(NODE_RIGHT(KEYWORD, RETURN))
                    {
                        if(root->right->right->type == NUMB)
                            fprintf(assembler_file, ":func_%d\npush %lf\nret\n", (int)root->left->value, root->right->right->value);
                        else
                            fprintf(assembler_file, ":func_%d\npush [%d]\nret\n", (int)root->left->value, (int)root->right->right->value);

                        return error;
                    }

                    is_declared_funcs[(int)root->left->value] = true;

                    fprintf(assembler_file, ":func_%d\n", (int)root->left->value);

                    error = emit_tree(root->right->left, assembler_file);

                    /*if(root->right->right->right->value == NUMB)
                        fprintf(assembler_file, "push %lf\nret\n", root->right->right->right->value);
                    else
                        fprintf(assembler_file, "push [%d]\nret\n", (int)root->right->right->right->value);*/
                    error = emit_tree(root->right->right->right, assembler_file);
                    fprintf(assembler_file, "ret\n");

                    break;
                }
                default: break;
                break;
            }
            break;
        }

        case OP:
        {
            switch((int)root->value)
            {
                case ASS:
                {
                   /* struct Node* cur_node = root->right;
                    if(root->right->type == FUN)
                    {
                        while(NODE_LEFT(OP, COM))
                        {
                            error = emit_node(cur_node->right);
                            cur_node = cur_node->left;
                        }

                        fprintf(assembler_file, "");
                    }
                    break;*/
                    if(root->left->type != VARIABLE)
                        return ERROR_IN_ASS;

                    error = emit_node(root->right, assembler_file);
                    fprintf(assembler_file, "pop [%d]\n", (int)root->left->value);
                    break;
                }

                case ADDF:
                {
                    error = emit_node(root->left, assembler_file);
                    error = emit_node(root->right, assembler_file);
                    fprintf(assembler_file, "add\n");
                    break;
                }

                case SUBF:
                {
                    error = emit_node(root->left, assembler_file);
                    error = emit_node(root->right, assembler_file);
                    fprintf(assembler_file, "sub\n");
                    break;
                }

                case MULF:
                {
                    error = emit_node(root->left, assembler_file);
                    error = emit_node(root->right, assembler_file);
                    fprintf(assembler_file, "mul\n");
                    break;
                }

                case DIVF:
                {
                    error = emit_node(root->left, assembler_file);
                    error = emit_node(root->right, assembler_file);
                    fprintf(assembler_file, "div\n");
                    break;
                }

                case POWF:
                {
                    error = emit_node(root->left, assembler_file);
                    error = emit_node(root->right, assembler_file);
                    fprintf(assembler_file, "pow\n");
                    break;
                }
                default: break;
            }
            break;
        }

        case FUN:
        {
            switch((int)root->value)
            {
                case NUMBER:
                {
                    fprintf(assembler_file, "push %lf\n", root->left->value);
                    break;
                }
                /*case SIN:
                {
                    fprintf(assembler_file, "push %d\n", sin(root->left->value));
                    break;
                }
                case COS:
                {
                    fprintf(assembler_file, "push %d\n", cos(root->left->value));
                    break;
                }
                case TAN:
                {
                    fprintf(assembler_file, "push %d\n", tan(root->left->value));
                    break;
                }
                case COT:
                {
                    fprintf(assembler_file, "push %d\n", 1/tan(root->left->value));
                    break;
                }

                case LN:
                {
                    fprintf(assembler_file, "push %d\n", log(root->left->value));
                    break;
                }*/

                case SCANF:
                {
                    if(root->left->type != VARIABLE)
                        return ERROR_IN_SCANF;

                    fprintf(assembler_file, "in\npop[%d]\n", (int)root->left->value);
                    break;
                }
                case PRINTF:
                {
                    if(root->left->type == VARIABLE)
                        fprintf(assembler_file, "push [%d]\n", (int)root->left->value);

                    if(root->left->type == NUMB)
                        fprintf(assembler_file, "push %lf\n", root->left->value);

                    fprintf(assembler_file, "out\n");

                    break;
                }

                default:
                {
                    cur_node = root->left;
                    while(NODE_LEFT(OP, COM))
                    {
                        error = emit_node(cur_node->right, assembler_file);
                        cur_node = cur_node->left;
                    }
                    error = emit_node(cur_node->left, assembler_file);

                    fprintf(assembler_file, "call :func_%d\n", (int)root->value);
                    break;
                }
            }
            break;
        }

        case VARIABLE:
        {
            fprintf(assembler_file, "push [%d]\n", (int)root->value);
            break;
        }
        case NUMB:
        {
            fprintf(assembler_file, "push %lf\n", root->value);
            break;
        }

        default:
            return ERROR_UNKNOWN_TYPE;
    }

    return error;
}
