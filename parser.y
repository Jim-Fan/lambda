%{
#include <stdio.h>
#include <string.h>
#include "lambda.h"

    /* Lexer would provide */
extern int yylex(void);
extern int yylineno;

    /* Parser should provide */
void yyerror(char* s)
{
    /* Can I do better than saying "ncl: syntax error" */
    /* fprintf(stderr, "ncl: %s\n", s); */

    /* Too bad, yytext is empty string */
    /* fprintf(stderr, "ncl: syntax error \"%s\"\n", yytext); */

    /* I am wondering how and if I should pass argument s */
    fprintf(stderr, "lambda: syntax error on line %d\n", yylineno);
}

%}

        /* typedef of YYSTYPE i.e. type of yylval */
%union {
    unsigned int n;
    unsigned int bif;
    unsigned int op;
    unsigned int e;
    unsigned int* a;
}

        /* Return value by lexer's rule, which is part of yylex() */
        /* Coded as C enum in parser header */
%token EOL L_BRACKET R_BRACKET VAR LAMBDA DOT
%token <n> NUMBER

        /* Associativity and precedence, not applicable here */
        /* This is a deep topic, definitely deserve investigation */
/* %nonassoc <fn> CMP */
/* %left TIMES DIV */

        /* Map data type of each grammatical construct */
/*
%type <bif> bif
%type <op> op
%type <e> scalar exp
%type <a> array
*/

        /* Top level production rule */
%start calclist

%%


exp:
    exp exp
    |
    lambda
    |
    VAR
    |
    NUMBER
    |
    L_BRACKET exp R_BRACKET
;

lambda:
    LAMBDA VAR DOT exp
;


        /****************    Parser main loop ***************/

calclist:

        /* Skip empty rule would fail bison */
        /* Why? Without empty rule, there is no terminal production
         * rule. And since calclist is recursively defined,
         * production would not terminate
         */
  %empty
  |
  calclist EOL {
    /* nothing to do? */
  }
  |
  calclist exp EOL {
    //ncl_append_inst($2);
  }
  |
  calclist error EOL {
    /* Using yyerrok allows recovery from syntax error and thus
     * checking on later lines is possible */
    yyerrok;

    /* Using YYABORT halts compiling upon first faulty line */
    //YYABORT;
  }
;


%%
