#ifndef TREE_SOFT_H
#define TREE_SOFT_H

#include <stdio.h>

struct Node{
    struct Node* left;
    int type;
    double value;
    struct Node* right;
};

#define DEFOP(FUNC, CODE, NAME)\
    FUNC = CODE,
#define DEFFUNC(FUNC, CODE, NAME)\
    FUNC = CODE,
#define DEFLOGIC(FUNC, CODE, NAME)\
    FUNC = CODE,
#define DEFKEYWORD(FUNC, CODE, NAME)\
    FUNC = CODE,

enum Type{
    /*NUMBER = 0,
    ADD = '+',
    SUB = '-',
    MUL = '*',
    DIV = '/',
    POW = '^',
    VAR = 'x',
    LN = 'l'*/
    #include "funcs.h"
    #undef DEFOP
    #undef DEFFUNC
    #undef DEFLOGIC
    #undef DEFKEYWORD
};

struct Node* create_node(int type, double value, struct Node* left = nullptr, struct Node* right = nullptr);
void push_node(struct Node* node, int type, double value);
//Node* delete_node(struct Node* node);
void delete_tree(struct Node* node);
void delete_tree_without_root(struct Node* node);
void tree_print(struct Node* tree);
void tree_print_inorder(FILE* file, struct Node* tree);
struct Node* copy_node(struct Node* node);
void optimizate_tree(struct Node* node);
bool is_number_tree(struct Node* node);
void merge_nodes(struct Node* in_node, struct Node* out_node);

int equal_double(double a, double b);

bool is_op_or_func_or_logic_or_keyword(int type);

double eval(struct Node* node);

#endif
