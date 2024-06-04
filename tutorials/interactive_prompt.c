#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <editline/history.h>

//static char input[2048];

int main(int argc, char** argv){
    //printing pretty stuff
    puts("RyanHasALisp Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    //endless loop  of interactions
    //fgets and fputs will cause ^[[D ... etc when arrow keys used.
    //thus need to use readline and addhistory
    while(1){
        //fputs("RyLisp> ", stdout);
        //fgets(input, 2048, stdin);
        //char* means string
        char* input = readline("RyLisp> ");
        add_history(input);
        printf("No U %s\n", input);
        free(input);
    }return 0;
}