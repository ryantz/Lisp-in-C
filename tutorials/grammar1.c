//adjective to describe a noun LOL
//parsing means analyzing and converting a program into an internal format
//that a runtime environment can actually run
#include <stdio.h>
#include <stdlib.h>

#include "mpc.c"
#include "mpc.h"

int main(int argc, char** argv){
    //parser 'adjective' to recognise descriptions(adjectives)
    //mpc_parser_t* is a parser, mpc_or means either ones are needed, mpc_sym is a wrapper
    mpc_parser_t* Adjective = mpc_or(4,
        mpc_sym("wow"), mpc_sym("many"),
        mpc_sym("so"), mpc_sym("such")
    );
    //parser 'noun' to recognize things(nouns)
    mpc_parser_t* Noun = mpc_or(5,
        mpc_sym("lisp"), mpc_sym("language"),
        mpc_sym("book"), mpc_sym("build"),
        mpc_sym("c")
    );
    //parser 'phrase'
    //mpc_and to specify one thing is required then another
    //mpcf_strfold (how to join) free (how to delete)
    mpc_parser_t* Phrase = mpc_and(2, mpcf_strfold,
        Adjective, Noun, free);
    //parser 'dodge'
    //mpc_many as 0 or more parsers needed
    mpc_parser_t* Dodge = mpc_many(mpcf_strfold, Phrase);

//TORUN: gcc -std=c99 -Wall grammar1.c mpc.c -ledit -lm -o parsing
}