#include <stdio.h>
#include <stdlib.h>

//if windows
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt){
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0'; //add 0 flag behind for end
    return cpy;
}

//windows does not need this no problem with arrow keys
void add_history(char* unused){}

//if not windows add libraries
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv){
    puts("Welcome to RyanHasALisp Version 0.0.0.0.1");
    puts("Press Ctrl+c to escape");

    while(1){
        char* input = readline("RyLisp> ");
        add_history(input);
        printf("No U %s\n", input);
        free(input);
    }return 0;
}

