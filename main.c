#include <stdio.h>
#include <getopt.h>
#include "lambda.h"

#ifdef YYDEBUG
extern int yydebug;
#endif

extern int yyparse();

void do_getopt(int, char**);

int main(int argc, char** argv)
{
    do_getopt(argc, argv);
    lambda_prompt();
    int result = yyparse();
    if (result != 0)
    {
        fprintf(stderr, "Aborted\n");
        goto CLEAN_UP;
    }

CLEAN_UP:
    //ncl_cleanup();
    return result;
}

void do_getopt(int argc, char** argv)
{
    int c = 0;
    while ((c = getopt(argc, argv, "t")) != -1)
    {
        switch (c)
        {
            // -t flags turns on parser tracing
            case 't':
                yydebug = 1;
                break;
            default:
                break;
        }
    }
}
