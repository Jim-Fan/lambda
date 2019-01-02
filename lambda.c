#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "lambda.h"

static struct binding* BINDING_HEAD = NULL;
static jmp_buf eval_err_jmpbuf;             // non-local escape of _eval()

struct node* new_node(NODE_TYPE node_type)
{
    static unsigned int NEXT_VAR_ID = 9000;

    struct node* nd = (struct node*)malloc(sizeof(struct node));
    nd->node_type = node_type;
    nd->var_id = ++NEXT_VAR_ID;         // TODO: create by gensym

    // Default values
    nd->num_value = 0;
    nd->var_name = '\0';
    nd->is_bound = false;   // unlike human, variables are born free
    nd->bound_by = 0;       // unless they meet tyranny "lambda"
    nd->left = NULL;
    nd->right = NULL;

    return nd;
}

struct node* new_num_node(unsigned int n)
{
    struct node* nd = new_node(NODE_TYPE_NUMBER);
    nd->num_value = n;
    return nd;
}

struct node* new_var_node(char c)
{
    struct node* nd = new_node(NODE_TYPE_VAR);
    nd->var_name = c;
    return nd;
}

struct node* new_lambda_node(struct node* var, struct node* exp)
{
    struct node* nd = new_node(NODE_TYPE_LAMBDA);

    // The variable in lambda abstraction is bounded
    // And is bounded to itself
    var->is_bound = true;
    var->bound_by = var->var_id;

    // Now exp is captured by tyranny "lambda"
    // Its free variables are now bound
    bound_var(var, exp);

    nd->left = var;
    nd->right = exp;
    return nd;
}

void bound_var(struct node* var, struct node* exp)
{
    if (exp->node_type == NODE_TYPE_VAR)
    {
        if ((! exp->is_bound) && var->var_name == exp->var_name)
        {
            exp->is_bound = true;
            exp->bound_by = var->var_id;
        }
    }
    else if (exp->node_type == NODE_TYPE_LAMBDA)
    {
        // Stop being naive
        if (var->var_name != exp->left->var_name)
        {
            bound_var(var, exp->right);
        }
    }
    else if (exp->node_type == NODE_TYPE_APP)
    {
        bound_var(var, exp->left);
        bound_var(var, exp->right);
    }
}

struct node* new_app_node(struct node* exp1, struct node* exp2)
{
    struct node* nd = new_node(NODE_TYPE_APP);
    nd->left = exp1;
    nd->right = exp2;
    return nd;
}

void _pprint(struct node* root, unsigned int depth, struct node* exp)
{
    if (exp->node_type == NODE_TYPE_VAR)
    {
        for (int i=0; i<depth; ++i) { putchar(' '); putchar(' '); }

        // if variable is bound but not to itself (e.g. x in (/x. E):
        //   lookup its value
        // else:
        //   it is free, just print it as-is
        if (exp->is_bound && exp->var_id != exp->bound_by)
        {
            struct binding* b = BINDING_HEAD;
            while (b != NULL)
            {
                if (exp->bound_by == b->var_id)
                {
                    // In current evaluation method, cyclic de-referencing could
                    // happen in subtle way. When this is detected, the bound
                    // variable is printed without following its value in binding
                    // list, as if it is a free variable

                    // Case 1:
                    // Combination ((S I) I) I can lead to /c.c where the inner
                    // c is ultimately bound to itself => cyclic pprint
                    if (b->value == root && depth > 0) goto FREE_OR_CYCLE_FOUND;

                    // Case 2:
                    // This is even more subtle. Consider S combinator applied to
                    // itself:
                    // (/s. s s) /x./y./z.(x z)(y z) ;
                    //
                    // <syntax tree>
                    // | APP(9019)
                    //   | LAMBDA(9005)
                    //     | VAR(9004, bound to 9004) s
                    //     | APP(9003)
                    //       | VAR(9001, bound to 9004) s
                    //       | VAR(9002, bound to 9004) s
                    //   | LAMBDA(9018)             <------------------------------
                    //     | VAR(9017, bound to 9017) x                           |
                    //     | LAMBDA(9016)           <===== pprint root            |
                    //       | VAR(9015, bound to 9015) y                         |
                    //       | LAMBDA(9014)                                       |
                    //         | VAR(9013, bound to 9013) z                       |
                    //         | APP(9012)                                        |
                    //           | APP(9008)                                      |
                    //             | VAR(9006, bound to 9017) x  <= loop back to --
                    //             | VAR(9007, bound to 9013) z
                    //           | APP(9011)
                    //             | VAR(9009, bound to 9015) y
                    //             | VAR(9010, bound to 9013) z
                    // <binding>
                    // var id: 9017    bound to: 9018
                    // var id: 9004    bound to: 9018
                    //
                    // De-referencing x (VAR 9006) jumps back to a node higher than
                    // pprint root, which can be trapped uglily:
                    if (b->value->node_type == NODE_TYPE_LAMBDA)
                    {
                        if (b->value->left->var_id == exp->bound_by)
                        {
                            goto FREE_OR_CYCLE_FOUND;
                        }
                    }

                    _pprint(root, depth, b->value);
                    return;
                }
                b = b->next;
            }
        }

FREE_OR_CYCLE_FOUND:
        printf(
            "| VAR(%d, %s %d) %c\n",
            exp->var_id,
            (exp->is_bound ? "bound to" : "free"),
            exp->bound_by,
            exp->var_name);
    }
    else if (exp->node_type == NODE_TYPE_LAMBDA)
    {
        for (int i=0; i<depth; ++i) { putchar(' '); putchar(' '); }
        printf("| LAMBDA(%d)\n", exp->var_id);
        _pprint(root, depth+1, exp->left);
        _pprint(root, depth+1, exp->right);
    }
    else if (exp->node_type == NODE_TYPE_APP)
    {
        for (int i=0; i<depth; ++i) { putchar(' '); putchar(' '); }
        printf("| APP(%d)\n", exp->var_id);
        _pprint(root, depth+1, exp->left);
        _pprint(root, depth+1, exp->right);
    }
    else if (exp->node_type == NODE_TYPE_NUMBER)
    {
        for (int i=0; i<depth; ++i) { putchar(' '); putchar(' '); }
        printf("| NUMBER(%d) %d\n", exp->var_id, exp->num_value);
    }
}

void pprint(struct node* exp)
{
    _pprint(exp, 0, exp);
}

void free_node(struct node* exp)
{
    // free children first, then the node itself
    if (exp->node_type == NODE_TYPE_VAR)
    {
        // no children
    }
    else if (exp->node_type == NODE_TYPE_LAMBDA)
    {
        free_node(exp->left);
        free_node(exp->right);
    }
    else if (exp->node_type == NODE_TYPE_APP)
    {
        free_node(exp->left);
        free_node(exp->right);
    }
    else if (exp->node_type == NODE_TYPE_NUMBER)
    {
        // no children
    }

    free(exp);
}

void handle_syntax_tree(struct node* exp)
{
    printf("<syntax tree>\n");
    pprint(exp);
    printf("\n");

    struct node* result = eval(exp);

    // Only print eval result and binding if no eval error
    if (result != NULL)
    {
        printf("<after>\n");

        // TODO: pprint *after eval* could cause non-termination and
        //       should catch and handle SIGINT
        pprint(result);
        printf("\n");

        printf("<binding>\n");
        dump_binding();
        printf("\n");
    }

    clear_binding();
    free_node(exp);
}

struct node* _eval(struct node* exp)
{
    struct binding* b = NULL;

    if (exp->node_type == NODE_TYPE_VAR)
    {
        // bound variable: look up binding list using var_id
        if (exp->is_bound)
        {
            b = BINDING_HEAD;
            while (b != NULL)
            {
                if (b->var_id == exp->bound_by) return b->value;
                b = b->next;
            }

            // TODO: Verify
            return exp;
        }

        // free variable: eval to itself
        else
        {
            return exp;
        }
    }
    else if (exp->node_type == NODE_TYPE_LAMBDA)
    {
        // nothing to do, really?
        return exp;
    }
    else if (exp->node_type == NODE_TYPE_APP)
    {
        // consider exp has the form: E1 E2

        // reduce E1 to the form of /k.E3 so that new binding
        // can be made
        struct node* E1 = _eval(exp->left);
        struct node* E2 = _eval(exp->right);

        // suppose exp->left is a VAR:
        //   after eval, if E1 is still a VAR, that means it is a free
        //   variable and is not an applicable term, thus evaluation error
        // suppose exp->left is a NUMBER:
        //   even more obvious, NUMBER is not applicable, evaluation error
        if (E1->node_type != NODE_TYPE_LAMBDA) {
            // TODO: semantics error, should long jump back to eval()
            //       since this is inside recursion
            printf("error(eval): expression is not applicable:\n");
            pprint(E1);
            longjmp(eval_err_jmpbuf, 1);
        }

        // create new binding k to E2
        b = BINDING_HEAD;
        BINDING_HEAD = (struct binding*) malloc(sizeof(struct binding));
        BINDING_HEAD->next = b;
        BINDING_HEAD->var_id = E1->left->var_id;
        BINDING_HEAD->value = deep_copy(E2);

        // evaluate body of E1, that is, E3
        return _eval(E1->right);
    }
    else if (exp->node_type == NODE_TYPE_NUMBER)
    {
        // nothing to do
        return exp;
    }
}

struct node* eval(struct node* exp)
{
    // clear binding from previous eval
    if (BINDING_HEAD != NULL)
    {
        struct binding* b = BINDING_HEAD;
        while (b != NULL)
        {
            BINDING_HEAD = BINDING_HEAD->next;
            free(b);
            b = BINDING_HEAD;
        }
    }
    BINDING_HEAD = NULL;

    // Jump back here if recursive eval finds error
    if (setjmp(eval_err_jmpbuf) != 0)
    {
        return NULL;
    }
    return _eval(exp);
}

struct node* deep_copy(struct node* nd)
{
    struct node* a = malloc(sizeof(struct node));
    memcpy(a, nd, sizeof(struct node));

    // distinguish naive or copied node
    //a->var_id += 8000000;

    if (nd->left != NULL) a->left = deep_copy(nd->left);
    if (nd->right != NULL) a->right = deep_copy(nd->right);

    return a;
}

void dump_binding()
{
    struct binding* b = BINDING_HEAD;
    while (b != NULL)
    {
        printf("var id: %d\tbound to: %d\n",
            b->var_id,
            b->value->var_id
        );
        b = b->next;
    }
}

void clear_binding()
{
    struct binding* b = BINDING_HEAD;
    struct binding* tmp = b;
    while (b != NULL)
    {
        b = tmp->next;
        free(tmp);
        tmp = b;
    }
    BINDING_HEAD = NULL;
}

void lambda_prompt()
{
    if (!isatty(fileno(stdin))) return;
    printf("> ");
    fflush(stdout);
}
