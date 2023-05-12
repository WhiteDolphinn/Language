#ifndef EXPRESSION_READER_H
#define EXPRESSION_READER_H

#include <stdio.h>
#include <assert.h>
#include "tree_soft.h"

#define MAX_VAR_LENGTH 20

int read_expession_rec_descent(FILE* source_file, Node** root);
void skip_spaces(char* expr , int* index);

enum Errors{
    SYNTAX_ERROR_IN_GET_G = 1,
    ERROR_ROOT_POINTER = 2,
    ERROR_READ_EXPRESSION = 3,
    ERROR_BUFFER_OVERFLOW = 4,
    SYNTAX_ERROR_IN_GET_N = 5,
    SYNTAX_ERROR_IN_GET_E = 6,
    SYNTAX_ERROR_IN_GET_T = 7,
    SYNTAX_ERROR_IN_GET_ID = 8,
    SYNTAX_ERROR_IN_GET_IF = 9,
    SYNTAX_ERROR_IN_GET_COMP = 10,
    SYNTAX_ERROR_IN_GET_MAIN = 11,
    SYNTAX_ERROR_IN_GET_SCANF = 12,
    SYNTAX_ERROR_IN_GET_PRINTF = 13,

    SYNTAX_ERROR = 257,
};

#endif
