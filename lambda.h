#ifndef _LAMBDA_H_
#define _LAMBDA_H_

#include "parser.tab.h"
#include <stdbool.h>

typedef enum {
    NODE_TYPE_NUMBER,
    NODE_TYPE_VAR,
    NODE_TYPE_LAMBDA,
    NODE_TYPE_APP
} NODE_TYPE;

struct node {
    NODE_TYPE node_type;
    unsigned int num_value;
    char var_name;
    unsigned int var_id;
    bool is_bound;
    unsigned int bound_by;
    struct node* left;
    struct node* right;
};

struct node* new_node(NODE_TYPE node_type);

struct node* new_num_node(unsigned int n);
struct node* new_var_node(char c);
struct node* new_lambda_node(struct node* var, struct node* exp);
void bound_var(struct node* var, struct node* exp);
struct node* new_app_node(struct node* exp1, struct node* exp2);

#endif
