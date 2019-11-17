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

                /* Once unknown char is found, eat everything up to EOE */
<UNKNOWN_CHAR>[^;]*    { fprintf(
                            stderr,
                            "lambda: unidentified token at \"%s\" on line %d\n",
                            yytext,
                            yylineno); }

                /* EOE turns off unknown char state */
<UNKNOWN_CHAR>";"        { BEGIN(INITIAL); return EOE; }

                /* Anything between # and EOL is comment */
"#"         { BEGIN(COMMENT); }
<COMMENT>.  { /* ignored */ }
<COMMENT>\n { BEGIN(INITIAL); return COMMENT_LINE; }

";"         { return EOE; }
"("         { return L_BRACKET; }
")"         { return R_BRACKET; }

[ \t\r\n]+    { /* Skip blanks */ }

[a-z]       { yylval.c = yytext[0]; return VAR; }
[A-Z]       { yylval.c = yytext[0]; return FUNC; }

.           { // Push back the offending char, let UNKNOWN_CHAR rules handle it
              yyless(yyleng-1);
              BEGIN(UNKNOWN_CHAR);
            }

%%
