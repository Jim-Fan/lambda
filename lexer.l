%option noyywrap yylineno case-sensitive

%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"

extern int fileno(const FILE*);
%}

                /* Define named regexp pattern here */

                /* Define lexer state variable here */
%x UNKNOWN_CHAR
%x COMMENT

%%

                /* Once unknown char is found, eat everything up to EOE */
                /* Yielding an UNKNOWN_TOKEN is necessary so as to trigger
                   a parse error, so that the error token and error rule
                   (if defined) will take effect, disgarding sub-expressions
                   on shift-reduce stack */
<UNKNOWN_CHAR>[^;]*    { printf("error(lex): unidentified token \"%s\"\n", yytext);
                         return UNKNOWN_TOKEN;
                       }

                /* EOE turns off unknown char state */
<UNKNOWN_CHAR>";"        { BEGIN(INITIAL); return EOE; }

                /* Anything between # and EOL is comment */
"#"         { BEGIN(COMMENT); }
<COMMENT>.  { /* ignored */ }
<COMMENT>\n { BEGIN(INITIAL); }

";"         { return EOE; }
"("         { return L_BRACKET; }
")"         { return R_BRACKET; }

[ \t\r\n]+    { /* Skip blanks */ }

                /* Number */
0|[1-9][0-9]*   { yylval.n = atoi(yytext);
                  return NUMBER;
                }

[a-z]       { yylval.c = yytext[0]; return VAR; }
"."         { return DOT; }
"/"         { return LAMBDA; }

.           { // Push back the offending char, let UNKNOWN_CHAR rules handle it
              yyless(yyleng-1);
              BEGIN(UNKNOWN_CHAR);
            }

%%
