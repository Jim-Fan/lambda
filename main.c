#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include "lambda.h"

#ifdef YYDEBUG
extern int yydebug;
yydebug = 1;
#endif

extern int yyparse();

extern LAMBDA_STATE lambda_state;

int main(void)
{
    if (signal(SIGINT, lambda_sig_handler) == SIG_ERR)
    {
        printf("lambda(sig): Error installing signal handler, continue\n");
    }

    lambda_prompt();
    lambda_state = LAMBDA_STATE_READING;
    int result = yyparse();
    return result;
}
