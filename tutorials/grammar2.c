#include <stdio.h>
#include <stdlib.h>

#include "mpc.c"
#include "mpc.h"

//parsing all at 1 time
int main(int argc, char** argv){
    mpc_parser_t* Adjective = mpc_new("adjective");
    mpc_parser_t* Noun = mpc_new("noun");
    mpc_parser_t* Phrase = mpc_new("phrase");
    mpc_parser_t* Doge = mpc_new("doge");

    mpca_lang(MPCA_LANG_DEFAULT,
    //multiline 
        "\
        adjective : \"wow\" | \"many\" | \"so\" | \"such\";\
        noun :  \"lisp\"| \"language\"| \"book\"| \"build\"| \"c\";\
        phrase : <adjective> <noun>;\
        doge : <phrase>*;\
        ", 
        Adjective, Noun, Phrase, Doge);
    

    //parse
    mpc_cleanup(4, Adjective, Noun, Phrase, Doge);
}