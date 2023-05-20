#ifndef BACKEND_H
#define BACKEND_H

#include <stdio.h>

int emit_prog(struct Node* root, FILE* assembler_file);
int emit_tree(struct Node* root, FILE* assembler_file);
int emit_node(struct Node* root, FILE* assembler_file);

enum backend_errors
{
    ERROR_NULL_ROOT = 1,
    ERROR_MAIN = 2,
    ERROR_NULL_NODE_PTR = 3,
    ERROR_NULL_LEFT_NODE_PTR = 4,
    ERROR_NULL_RIGHT_NODE_PTR = 5,
    ERROR_UNKNOWN_TYPE = 6,
    ERROR_REDEFINATION_VAR = 7,
    ERROR_REDEFINATION_FUNC = 8,
    ERROR_IN_ASS = 9,
    ERROR_IN_SCANF = 10,
};

#endif
