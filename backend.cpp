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
    int error = 0;
    //printf("%d %lf\n", cur_node->type, cur_node->value);

    if(!NODE(KEYWORD, MAI))
        return ERROR_MAIN;

    //printf("%d %lf\n", cur_node->left->type, cur_node->left->value);

    if(NODE_LEFT(OP, AND))
    {
        error = emit_tree(cur_node->left, assembler_file);
        fprintf(assembler_file, "hlt\n");
        return error;
    }

    error = emit_node(cur_node->left, assembler_file);
    fprintf(assembler_file, "hlt\n");
    return error;
}

int emit_tree(struct Node* root, FILE* assembler_file)
{
    static int error = 0;
    if(error != 0)
        return error;

    if(root == nullptr)
        return ERROR_NULL_NODE_PTR;

    struct Node* cur_node = root;

    /*if(cur_node->left == nullptr)
        return ERROR_NULL_LEFT_NODE_PTR;

    if(cur_node->right == nullptr)
        return ERROR_NULL_RIGHT_NODE_PTR;*/

    if(NODE(OP, AND) || NODE(OP, COM))
    {
        error = emit_tree(cur_node->left, assembler_file);
        error = emit_node(cur_node->right, assembler_file);
    }

    error = emit_node(cur_node, assembler_file);
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
                    printf("var: %d\t val: %lf\n", (int)root->left->value, root->right->value);
                    fprintf(assembler_file, "pop [%d]\n", (int)root->left->value);
                    break;
                }
                case DEC:
                {
                    if(is_declared_funcs[(int)root->left->value])
                        return ERROR_REDEFINATION_FUNC;

                    fprintf(assembler_file, ":func_%d\n", (int)root->left->value);
                    cur_node = root->left->left;

                    while(NODE(OP, COM))
                    {
                        if(cur_node->right->type == VARIABLE)
                            fprintf(assembler_file, "pop [%d]\n", (int)cur_node->right->value);
                        else
                            return ERROR_IN_ARGS_IN_FUN;

                        cur_node = cur_node->left;
                    }
                    fprintf(assembler_file, "pop [%d]\n", (int)cur_node->value);

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

        case LOGIC:
        {
            //case IF:
            static int num_of_if = 0;
            cur_node = root;
            fprintf(assembler_file, ":if_%d\n", num_of_if);

            if(root->left->type == VARIABLE)
            {
                fprintf(assembler_file, "push 0\npush [%d]\njne :end_if_%d\n", (int)root->left->value, num_of_if);
                error = emit_tree(root->right, assembler_file);
                fprintf(assembler_file, ":end_if_%d\n", num_of_if);
            }

            if(NODE_LEFT(OP, MOR) || NODE_LEFT(OP, MOE) || NODE_LEFT(OP, EQU) || NODE_LEFT(OP, NEQ))
            {
                error = emit_node(root->left->left, assembler_file);
                error = emit_node(root->left->right, assembler_file);

                switch((int)root->left->value)
                {
                    case MOR:   fprintf(assembler_file, "ja :end_of_if_%d\n", num_of_if);   break;
                    case MOE:   fprintf(assembler_file, "jae :end_of_if_%d\n", num_of_if);  break;
                    case EQU:   fprintf(assembler_file, "je :end_of_if_%d\n", num_of_if);   break;
                    case NEQ:   fprintf(assembler_file, "jne :end_of_if_%d\n", num_of_if);  break;
                    default:    fprintf(assembler_file, "jmp :end_of_if_%d\n", num_of_if);   break;
                }
            }

            num_of_if++;
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
                    if(NODE(OP, COM))
                    {
                        error = emit_tree(cur_node->left, assembler_file);
                        error = emit_node(cur_node->right, assembler_file);
                    }

                    fprintf(assembler_file, "call :func_%d\n", (int)root->value);

                    cur_node = root;
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
