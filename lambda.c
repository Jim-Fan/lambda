#include <stdio.h>
#include <stdlib.h>
#include "lambda.h"

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
