#include "tree_soft.h"
#include "expression_reader.h"
#include "lexer.h"
#include "text.h"
#include "backend.h"
#include <stdlib.h>

static void help();

int main(int argc, const char* argv[])
{
    if(argc != 2)
    {
        help();
        return 0;
    }

    const char* name_of_source_file = argv[1];

    struct Node* n1 = nullptr;
    int error = 0;

    //system ("ls");
    FILE* source_file = fopen(name_of_source_file, "r");
    assert(source_file);

    FILE* assembler_file = fopen("./CPU/Assembler/proga.asm", "w");
    if(source_file == nullptr)
    {
        printf("I can't open source_file\n");
        return 0;
    }

    if(assembler_file == nullptr)
    {
        printf("I can't open assembler_file\n");
        fclose(source_file);
        return 0;
    }

    //char* source_str = (char*)calloc(2048, sizeof(char));
    char* source_str = text_reader(source_file, name_of_source_file);

    char** var_table = (char**)calloc(1, sizeof(char) * MAX_VAR_LENGTH * MAX_NUM_OF_VARS);
    char** func_table = (char**)calloc(1, sizeof(char) * MAX_FUNC_LENGTH * MAX_NUM_OF_FUNCS);
    struct token* tokens = (struct token*)calloc(MAX_NUM_OF_TOKENS, sizeof(struct token));

    {

    if(var_table == nullptr)
    {
        printf("error in calloc var_table\n");
        error = 1;
        goto exit;
    }
    if(func_table == nullptr)
    {
        printf("error in calloc func_table\n");
        error = 3;
        goto exit;
    }

    if(!tokenizator(tokens, source_str, var_table, func_table))
    {
        printf("Error in tokeniaztor\n");
        error = 2;
        goto exit;
    }

    for(int i = 0; i < MAX_NUM_OF_TOKENS; i++)
        if(tokens[i].type != 0 || tokens[i].value.int_val != 0)
        {
            if(tokens[i].type != NUMB)
                printf("tokens[%d].type = %d\t.value = %d\n", i, tokens[i].type, tokens[i].value.int_val);
            else
                printf("tokens[%d].type = %d\t.value = %.3lf\n", i, tokens[i].type, tokens[i].value.double_val);
        }


    /*if(read_expession_rec_descent(source_file, &n1) == 0)
    {
        tree_print(n1);
    }*/
    error = convert_tokens_to_ast(tokens, &n1);
    if(error != 0)
    {
        printf("Error in convert_tokens_to_ast. Code %d.\n", error);
        goto exit;
    }



    error = emit_prog(n1, assembler_file);
    if(error != 0)
    {
        printf("Error in emit_prog. Code %d.\n", error);
        goto exit;
    }

    }

    exit:
    tree_print(n1);
    fclose(source_file);
    fclose(assembler_file);
    free(var_table);
    free(func_table);
    free(source_str);
    free(tokens);
    delete_tree(n1);

    return error;
}


static void help()
{
    printf("./a.out\t<name of .sav file>\n");
}
