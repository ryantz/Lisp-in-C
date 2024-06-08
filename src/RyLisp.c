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

/*
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
*/

//error handling
//to establish if it is an error or not
typedef struct{
    int type;
    long num;
    int err;
}lval; //lval, lisp value

//enumerations for types and errors
enum{ LVAL_NUM, LVAL_ERR }; //either a number or an error
enum{ LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM }; // divide by 0, unknown operator, number too large for long

lval lval_num(long x){
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

lval lval_err(int x){
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

//printing errors
void lval_print(lval v){
    switch(v.type){
        case LVAL_NUM: printf("%li", v.num); break;
        case LVAL_ERR:
            if(v.err == LERR_DIV_ZERO){
                printf("Error: Divison By Zero!");
            }
            if(v.err == LERR_BAD_OP){
                printf("Error: Invalid Operator!");
            }
            if(v.err == LERR_BAD_NUM){
                printf("Error: Invalid Number!");
            }break;
    }
}

void lval_println(lval v){
    lval_print(v);
    putchar('\n'); //just a function to do lval_print and start new line
}

//evaluating added with errors and using lval
lval eval_operation(lval x, char* operator, lval y){
    if(x.type == LVAL_ERR){return x;}
    if(y.type == LVAL_ERR){return y;}

    if(strcmp(operator, "+") == 0){
        return lval_num(x.num + y.num);
    }
    if(strcmp(operator, "*") == 0){
        return lval_num(x.num * y.num);
    }
    if(strcmp(operator, "-") == 0){
        return lval_num(x.num - y.num);
    }
    if(strcmp(operator, "/") == 0){
        return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num/y.num);
    }return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t){
    if(strstr(t->tag, "number")){
        //checking if there is error when converting
        errno = 0;
        //strtol converting string to long (str, endptr, base)
        long x = strtol(t->contents, NULL, 10);
        //ERANGE  represents range error
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }
    char* operator = t->children[1]->contents;
    lval x = eval(t->children[2]);

    int i = 3;
    while(strstr(t->children[i]->tag, "expression")){
        x = eval_operation(x, operator, eval(t->children[i]));
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

            //long result = eval(r.output);
            //printf("%li\n", result);

            lval result = eval(r.output);
            lval_println(result);

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

