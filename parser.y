%define parse.error verbose

%{
#include <stdio.h>
#include <string.h>
#include "lambda.h"

    /* Lexer would provide */
extern int yylex(void);
extern int yylineno;

    /* Parser should provide */
void yyerror(char* err_msg)
{
    printf("%s\n", err_msg);
}

%}

        /* typedef of YYSTYPE i.e. type of yylval */
%union {
    unsigned int n;
    unsigned int bif;
    unsigned int op;
    unsigned int e;
    unsigned int* a;
    char c;
    struct node* nd;
}

        /* Return value by lexer's rule, which is part of yylex() */
        /* Coded as C enum in parser header */
%token COMMENT_LINE EOE EOL L_BRACKET R_BRACKET LAMBDA DOT
%token <c> VAR
%token <n> NUMBER

        /* Associativity and precedence, not applicable here */
        /* This is a deep topic, definitely deserve investigation */
/* %nonassoc <fn> CMP */
/* %left TIMES DIV */

        /* Map data type of each grammatical construct */
%type <nd> exp
%type <nd> app
%type <nd> lambda

        /* Top level production rule */
%start calclist

%%


exp:
    app
    |
    lambda
    |
    VAR         { $$ = new_var_node($1); }
    |
    NUMBER      { $$ = new_num_node($1); }
    |
    L_BRACKET exp R_BRACKET     { $$ = $2; }
;

lambda:
    LAMBDA VAR DOT exp  { $$ = new_lambda_node(new_var_node($2), $4); }
;

app:
    exp exp     { $$ = new_app_node($1, $2); }
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
    handle_syntax_tree($2);
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
