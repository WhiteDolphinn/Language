#include "backend.h"
#include "tree_soft.h"
#include "lexer.h"
#include "soft_cpu.h"

int emit_prog(struct Node* root, FILE* assembler_file)
{
    if(root == nullptr)
        return ERROR_NULL_ROOT;

    struct Node* cur_node = root;

    if(!NODE_LEFT(KEYWORD, MAI))
        return ERROR_MAIN;

    if(NODE(OP, AND))
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

    struct Node* cur_node = root->left;

    if(cur_node->left == nullptr)
        return ERROR_NULL_LEFT_NODE_PTR;

    if(cur_node->right == nullptr)
        return ERROR_NULL_RIGHT_NODE_PTR;

    if(NODE(OP, AND))
        error = emit_tree(cur_node->left, assembler_file);
    else
        error = emit_node(cur_node->left, assembler_file);

    error = emit_node(cur_node->right, assembler_file);
    return error;
}

int emit_node(struct Node* root, FILE* assembler_file)
{
    static bool is_declared_vars[MAX_NUM_OF_VARS] = {};
    static bool is_declared_funcs[MAX_NUM_OF_FUNCS] = {};
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
                case RETURN:
                {
                    break;
                }
                case VAR:
                {
                    if(is_declared_vars[(int)root->left->value])
                        return ERROR_REDEFINATION_VAR;

                    is_declared_vars[(int)root->left->value] = true;

                    fprintf(assembler_file, "push %d\npop [%d]\n", (int)root->right->value, (int)root->left->value);
                    break;
                }
                case DEC:
                {
                    if(is_declared_funcs[(int)root->left->value])
                        return ERROR_REDEFINATION_FUNC;

                    while(NODE_LEFT(OP, COM))
                    {
                        if(cur_node->right->type == VARIABLE)
                    }

                    cur_node = root;

                    if(NODE_RIGHT(KEYWORD, RETURN))
                    {
                        if(root->right->right->type == NUMB)
                            fprintf(assembler_file, ":func_%d\npush %d\nret\n", (int)root->left->value, root->right->right->value);
                        else
                            fprintf(assembler_file, ":func_%d\npush [%d]\nret\n", (int)root->left->value, root->right->right->value);

                        return error;
                    }

                    is_declared_funcs[(int)root->left->value] = true;

                    fprintf(assembler_file, ":func_%d\n", (int)root->left->value);

                    error = emit_tree(root->right->left, assembler_file);

                    if(root->right->right->right->value == NUMB)
                        fprintf(assembler_file, "push %d\nret\n", root->right->right->right->value);
                    else
                        fprintf(assembler_file, "push [%d]\nret\n", root->right->right->right->value);

                    break;
                }
                break;
            }
        }

        default:
            return ERROR_UNKNOWN_TYPE;
    }

    return error;
}
