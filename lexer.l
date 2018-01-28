%option noyywrap yylineno case-sensitive

%{
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parser.tab.h"

// Hack YY_INPUT by incorporating readline()
// if(..) { /* if-block */ } is hacked by me
// else { /* else-block */ } is nearly directly copied from original
// YY_INPUT expansion, which works when input is non-interactive
static int my_yy_input(char* buf, int max_size)
{
    int result = YY_NULL;

    if (YY_CURRENT_BUFFER_LVALUE->yy_is_interactive)
    {
        char *line = NULL;

        // TODO: Signal handling
        line = readline("> ");
        if (!line) return YY_NULL;

        if(strlen(line) >= max_size-1) // reserve one byte for char(10)
        {
            // TODO: Not good enough, yy_fatal_error calls exit() straight away
            yy_fatal_error("input line of scanner overflows, abort\n");
        }

        // Should give char(10) back, or not? Affirmative
        // Otherwise a comment line started with '#' cannot be
        // reduced to a COMMENT_LINE token, prompt will be stucked
        sprintf(buf, "%s\n", line);

        add_history(line);
        free(line);

        return strlen(buf);
    }
    else
    {
        errno=0;
        while ( (result = (int) fread(buf, 1, (yy_size_t) max_size, yyin)) == 0 && ferror(yyin))
        {
            if( errno != EINTR)
            {
                // YY_INPUT was using YY_FATAL_ERROR
                // TODO: Also not good enough
                yy_fatal_error("input in flex scanner failed");
                break;
            }
            errno=0;
            clearerr(yyin);
        }
    }

    return result;
}

#undef YY_INPUT
#ifndef YY_INPUT
#define YY_INPUT(buf,result,max_size) (result=my_yy_input(buf, max_size))
#endif

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
