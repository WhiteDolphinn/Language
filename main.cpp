#include "tree_soft.h"
#include "expression_reader.h"
#include "lexer.h"

int main()
{
    struct Node* n1 = nullptr;
    FILE* source_file = fopen("file.sav", "r");

    if(source_file == nullptr)
    {
        printf("I can't open source_file\n");
        return 0;
    }

    if(read_expession_rec_descent(source_file, &n1) == 0)
    {
        tree_print(n1);
    }

    delete_tree(n1);

    return 0;
}
