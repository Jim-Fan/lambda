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
    fprintf(stderr, "lambda: syntax error on line %d\n", yylineno);
}

%}

        /* typedef of YYSTYPE i.e. type of yylval */
%union {
    char c;
}

        /* Return value by lexer's rule, which is part of yylex() */
        /* Coded as C enum in parser header */
%token COMMENT_LINE EOE L_BRACKET R_BRACKET
%token <c> VAR FUNC

        /* Associativity and precedence */
%left FUNC

        /* Top level production rule */
%start calclist

%printer { fprintf(yyo, "%c", $$); } VAR;
%printer { fprintf(yyo, "%c", $$); } FUNC;
%printer { fprintf(yyo, "%p", $$); } exp;
%printer { fprintf(yyo, "%p", $$); } application;
%printer { fprintf(yyo, "%p", $$); } monadic;
%printer { fprintf(yyo, "%p", $$); } dyadic;
%%


exp:
    application
    |
    VAR
    |
    L_BRACKET exp R_BRACKET
;

application:
    monadic
    |
    dyadic
;

monadic:
    FUNC exp
;

dyadic:
    exp FUNC exp
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
  calclist COMMENT_LINE {
    // nothing to do
  }
  |
  calclist exp EOE {
    //ncl_append_inst($2);
  }
  |
  calclist error EOE {
    /* Using yyerrok allows recovery from syntax error and thus
     * checking on later lines is possible */
    yyerrok;

    /* Using YYABORT halts compiling upon first faulty line */
    //YYABORT;
  }
;


%%
