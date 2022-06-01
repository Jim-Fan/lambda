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

struct ast_node {
    NODE_TYPE node_type;
    unsigned int num_value;
    char var_name;
    unsigned int var_id;
    bool is_bound;
    unsigned int bound_by;
    struct ast_node* left;
    struct ast_node* right;
};

struct binding {
    unsigned int var_id;
    struct ast_node* value;
    struct binding* next;
};

struct ast_node* new_node(NODE_TYPE node_type);

struct ast_node* new_num_node(unsigned int n);
struct ast_node* new_var_node(char c);
struct ast_node* new_lambda_node(struct ast_node* var, struct ast_node* exp);
void bound_var(struct ast_node* var, struct ast_node* exp);
struct ast_node* new_app_node(struct ast_node* exp1, struct ast_node* exp2);

void _pprint(struct ast_node* root, unsigned int depth, struct ast_node* exp);
void pprint(struct ast_node* exp);

void free_node(struct ast_node* exp);
void handle_syntax_tree(struct ast_node* exp);

struct ast_node* apply(struct ast_node* lambda, struct ast_node* arg);
struct ast_node* _eval(struct ast_node* exp);
struct ast_node* eval(struct ast_node* exp);

void dump_binding();
void clear_binding();

void lambda_prompt();
#endif
