//implement polish notation
//operator > numbers / paranthesis 
//seeing how the code converts to understandable code for itself
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

//recursion for number of nodes
int number_of_nodes(mpc_ast_t* t){

    //base case
    if (t->children_num == 0){
        return 1;
    }

    if (t->children_num >= 1){
        int total = 1;
        for(int i = 0; i < t->children_num; i++){
            //recurse
            total = total + number_of_nodes(t->children[i]);
        }return total;
    }return 0;
}

long eval_operation(long x, char* oper, long y){
    //string compare
    if(strcmp(oper, "+")==0){
        return x + y;
    }
    if(strcmp(oper, "*")==0){
        return x * y;
    }
    if(strcmp(oper, "-")==0){
        return x - y;
    }
    if(strcmp(oper, "/")==0){
        return x / y;
    }
    return 0;
}

//evaluation of nodes
long eval(mpc_ast_t* t){
    if(strstr(t->tag, "number")){

        //converting char* into int 
        return atoi(t->contents);
    }

    //operator is second child
    char* oper = t->children[1]->contents;

    //store third child in x
    long x = eval(t->children[2]);

    int i = 3;
    while(strstr(t->children[i]->tag, "expression")){
        x = eval_operation(x, oper, eval(t->children[i]));
        i++;
    }return x;
}


int main(int argc, char** argv){

//parsing, take in input, translate into something machine can understand
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

            long result = eval(r.output);
            printf("%li\n", result);

            /*
            //loading AST from output (Abstract Syntax Tree)
            mpc_ast_t* a = r.output;
            printf("Tag: %s\n", a->tag); //tag is in mpc.h..->used as ref pointer types
            printf("Contents: %s\n", a->contents);
            printf("Number of children: %i\n", a->children_num);

            //getting first child
            mpc_ast_t* child0 = a->children[0];
            printf("First child Tag: %s\n", child0->tag); 
            printf("First child Contents: %s\n", child0->contents);
            printf("First child Number of children: %i\n", child0->children_num);
            */
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

