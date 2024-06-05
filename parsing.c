//implement polish notation
//operator > numbers / paranthesis 
#include <stdio.h>
#include <stdlib.h>
#include "mpc.c"
#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt){
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}

void add_history(char* unused){}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv){

    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expression = mpc_new("expression");
    mpc_parser_t* Polish = mpc_new("polish");

    mpca_lang(MPCA_LANG_DEFAULT,
        "\
            number: /-?[0-9]+/; \
            operator: '+' | '-' | '*' | '/' ; \
            expression: <number> | '(' <operator> <expression>+ ')'; \
            polish: /^/ <operator> <expression>+ /$/ ; \
        ", 
        Number, Operator, Expression, Polish);

    puts("RyanHasALisp Version 0.0.0.0.2");
    puts("To exit program please press Ctrl+c :)");

    while(1){
        //read the expression entered by user
        char* input = readline("RyLisp> ");
        add_history(input);

        //parse user input
        mpc_result_t r;
        if(mpc_parse("<stdin>", input, Polish, &r)){
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        }else{
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
    }
    mpc_cleanup(4, Number, Operator, Expression, Polish);
    return 0;

    //gcc -std=c99 -Wall parsing.c -ledit -o parse
}

