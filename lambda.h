#ifndef _LAMBDA_H_
#define _LAMBDA_H_

#include "parser.tab.h"
#include <stdbool.h>
#include <stdio.h>

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

struct binding {
    unsigned int var_id;
    struct node* value;
    struct binding* next;
};

struct node* new_node(NODE_TYPE node_type);

struct node* new_num_node(unsigned int n);
struct node* new_var_node(char c);
struct node* new_lambda_node(struct node* var, struct node* exp);
void bound_var(struct node* var, struct node* exp);
struct node* new_app_node(struct node* exp1, struct node* exp2);

void _pprint(struct node* root, unsigned int depth, struct node* exp);
void pprint(struct node* exp);

void free_node(struct node* exp);
void handle_syntax_tree(struct node* exp);

struct node* apply(struct node* lambda, struct node* arg);
struct node* _eval(struct node* exp);
struct node* eval(struct node* exp);

void dump_binding();
void clear_binding();

void lambda_prompt();
#endif
