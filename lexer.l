%option noyywrap yylineno case-sensitive

%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
%}

                /* Define named regexp pattern here */

                /* Define lexer state variable here */
%x UNKNOWN_CHAR
%x COMMENT

%%

                /* Once unknown char is found, eat everything up to EOL */
<UNKNOWN_CHAR>[^\n]*    { fprintf(
                            stderr,
                            "ncl: unidentified token at \"%s\" on line %d\n",
                            yytext,
                            yylineno); }

                /* EOL turns off unknown char state */
<UNKNOWN_CHAR>\n        { BEGIN(INITIAL); return EOL; }

                /* Anything between # and EOL is comment */
"#"         { BEGIN(COMMENT); }
<COMMENT>.  { /* ignored */ }
<COMMENT>\n { BEGIN(INITIAL); return EOL; }

\n          { return EOL; }
"("         { return L_BRACKET; }
")"         { return R_BRACKET; }

[ \t\r]+    { /* Skip blanks */ }

                /* Number */
0|[1-9][0-9]*   { yylval.n = atoi(yytext);
                  return NUMBER;
                }

[a-z]       { return VAR; }
"."         { return DOT; }
"/"         { return LAMBDA; }

.           { // Push back the offending char, let UNKNOWN_CHAR rules handle it
              yyless(yyleng-1);
              BEGIN(UNKNOWN_CHAR);
            }

%%
