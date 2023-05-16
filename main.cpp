#include "tree_soft.h"
#include "expression_reader.h"
#include "lexer.h"
#include "text.h"
#include <stdlib.h>

int main()
{
    struct Node* n1 = nullptr;
    FILE* source_file = fopen("file.sav", "r");

    if(source_file == nullptr)
    {
        printf("I can't open source_file\n");
        return 0;
    }

    //char* source_str = (char*)calloc(2048, sizeof(char));
    char* source_str = text_reader(source_file, "file.sav");

    char** var_table = (char**)calloc(1, sizeof(char) * MAX_VAR_LENGTH * MAX_NUM_OF_VARS);
    if(var_table == nullptr)
    {
        printf("error in calloc var_table\n");
        free(source_str);
        return 1;
    }

    struct token* tokens = (struct token*)calloc(1000, sizeof(struct token));
    tokenizator(tokens, source_str, var_table);

    for(int i = 0; i < 15; i++)
        printf("tokens[%d].type = %d\t.value = %d\n", i, tokens[i].type, tokens[i].value);

    /*if(read_expession_rec_descent(source_file, &n1) == 0)
    {
        tree_print(n1);
    }*/

    free(var_table);
    free(source_str);
    free(tokens);
    delete_tree(n1);

    return 0;
}
