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


//error handling
//to establish if it is an error or not
//adding new params for Sexpr
typedef struct lval{
    int type;
    long num;
    //int err;
    //Error and symbol types have string data now
    char* err;
    char* sym;
    //count and pointer to a list of lval*, points to other individual lval
    int count;
    struct lval** cell;
}lval; //lval, lisp value

//enumerations for types and errors
//added new lval types for S-expressions symbols (+-*/) and sexpression
enum{ LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR }; //either a number or an error


//TODO CONSTRUCTORS AND DESTRUCTORS
//creating new lval* numbers 
lval* lval_num(long x){
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

lval* lval_err(char* m){
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m) + 1); // +1 because of \0 at he end? strlen returns excluding end flag
    stcpy(v->err, m);
    return v;
}

lval* lval_sym(char* s){
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval* lval_sexpr(void){
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

//releasing memory acquired from malloc
void lval_del(lval* v){
    switch(v->type){
        case LVAL_NUM: break;

        //err and sym uses malloc to store strlen +1
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;
        case LVAL_SEXPR: 
            for(int i=0; i< v->count; i++){
                //del all elements in sexpr
                lval_del(v->cell[i]);
            }
            //free memory allocated to contain the pointers
            free(v->cell); 
            break;
    }free(v); //free memory allocated for the lval struct
}

//reading expressions
lval* lval_read_num(mpc_ast_t* t){
    errno = 0;
    long x = strtol(t->contents ,NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval* lval_add(lval* v, lval* x){
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count-1] = x;
    return v;
}

lval* lval_read(mpc_ast_t* t){
    //if sym or num
    if(strstr(t->tag, "number")){
        return lval_read_num(t);
    }
    if(strstr(t->tag, "symbol")){
        return lval_sym(t->contents);
    }

    //if root >  or sexpr
    lval* x = NULL;
    if(strcmp(t->tag, ">") == 0){
        x = lval_sexpr();
    }
    if(strstr(t->tag, "sexpr")){
        x = lval_sexpr();
    }

    for(int i=0; i< t->children_num; i++){
        if(strcmp(t->children[i]->contents, "(") == 0){
            continue;
        }
        if(strcmp(t->children[i]->contents, ")") == 0){
            continue;
        }
        if(strcmp(t->children[i]->tag, "regex") == 0){
            continue;
        }
        
        x = lval_add(x, lval_read(t->children[i]));
    }
    return x;
}

//printing expressions
void lval_expr_print(lval* v, char* open, char* close){
    putchar(open);
    for(int i=0; i< v->count; i++){
        //print value contained within
        lval_print(v->cell[i]);

        //dont print trailing space if it is last elemnt
        if(i != (v->count-1)){
            putchar(' ');
        }
    }
    putchar(close);
}

void lval_print(lval* v){
    switch(v->type){
        case LVAL_NUM: printf("%li", v->num);break;
        case LVAL_ERR: printf("Error: %s", v->err);break;
        case LVAL_SYM: printf("%s", v->sym);break;
        case LVAL_SEXPR: lval_expr_print(v, '(', ')');break;
    }
}

void lval_println(lval* v){
    lval_print(v);
    putchar('\n');
}

//evaluating expressions



int main(int argc, char** argv){

//S-expression
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* Sexpr = mpc_new("sexpr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
        "\
            number: /-?[0-9]+/; \
            symbol: '+' | '-' | '*' | '/' |; \
            sexpr: '(' <expr>* ')' ; \
            expr: <number> | <symbol> | <sexpr>; \
            lispy: /^/ <expr>* /$/; \
        ", Number, Symbol, Sexpr, Expr, Lispy);

    puts("RyanHasALisp Version 0.0.0.0.2");
    puts("To exit program please press Ctrl+c :)");

    while(1){
        //read the expression entered by user
        char* input = readline("RyLisp> ");
        add_history(input);
        
        //parse user input
        mpc_result_t r;
        if(mpc_parse("<stdin>", input, Lispy, &r)){
            lval* x = lval_eval(lval_read(r.output));
            lval_println(x);
            lval_del(x);
            mpc_ast_delete(r.output);
        }else{
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
        
    }
    mpc_cleanup(4, Number, Symbol, Sexpr, Expr, Lispy);
    return 0;
}

