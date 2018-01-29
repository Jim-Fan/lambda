#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "lambda.h"

static struct binding* BINDING_HEAD = NULL;
static jmp_buf eval_jbuf;

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
                    // Cyclic expression, see ((S I) I) I in example/combinator.lam
                    // Silently exit lookup, print the variable as-is
                    if (b->value == root && depth > 0) goto FREE_OR_SELF_BOUNDED;

                    _pprint(root, depth, b->value);
                    return;
                }
                b = b->next;
            }
        }

FREE_OR_SELF_BOUNDED:
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

    printf("<after>\n");
    if (result != NULL) pprint(result);
    printf("\n");

    printf("<binding>\n");
    dump_binding();
    printf("\n");

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

        // if E1 is free, further eval is no possible
        // otherwise it is bound, thus E1 is lambda construct
        // TODO: returning exp hides eval result E2
        //       not necessarily, modify pprint to do binding lookup
        //       on bound var
        if (E1->node_type == NODE_TYPE_VAR && E1->is_bound == false) {
            return exp;
        }

        if (E1->node_type != NODE_TYPE_LAMBDA) {
            // TODO: semantics error, should long jump back to eval()
            //       since this is inside recursion
            printf("lambda: expression is not applicable:\n");
            pprint(E1);
            longjmp(eval_jbuf, 1);
        }

        // create new binding k to E2
        b = BINDING_HEAD;
        BINDING_HEAD = (struct binding*) malloc(sizeof(struct binding));
        BINDING_HEAD->next = b;
        BINDING_HEAD->var_id = E1->left->var_id;
        BINDING_HEAD->value = E2;

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

    if (setjmp(eval_jbuf) != 0)
    {
        // error during recursive eval
        return NULL;
    }
    return _eval(exp);
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
