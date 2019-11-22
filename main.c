#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include "lambda.h"

#ifdef YYDEBUG
extern int yydebug;
#endif

extern int yyparse();

static jmp_buf jbuf;

static void sig_handler(int signal)
{
    if (signal == SIGINT) longjmp(jbuf, 1);
}

int main(void)
{
#ifdef YYDEBUG
    yydebug = 1;
#endif

    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        fprintf(stderr, "Error installing signal handler, continue\n");
    }

    // Crucial to setjmp early, if this is put, for example,
    // after CLEAN_UP label, longjmp() has no place to jump
    // to when interrupted during execution loop
    if (setjmp(jbuf) != 0)
    {
        fprintf(stderr, "Interrupted, abort and clean up...\n");
        goto CLEAN_UP;
    }

    //ncl_init();
    printf("(lambda version 0.1)\n");

    int result = yyparse();
    if (result != 0)
    {
        fprintf(stderr, "Aborted\n");
        goto CLEAN_UP;
    }

    /*
    result = ncl_resolve_labels();
    if (result == 0)
    {
        ncl_exec_inst();
        ncl_dump_reg();
    }
    else
    {
        fprintf(stderr, "Aborted\n");
    }
    */

CLEAN_UP:
    //ncl_cleanup();
    return result;
}
