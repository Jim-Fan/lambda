#ifndef _SECD_H_
#define _SECD_H_

#include "../lambda.h"

// SECD
void secd_init();
void secd_eval(struct ast_node*);
void secd_destroy();

// RPN
typedef enum {
    RPN_NODE_TYPE_AP,
    RPN_NODE_TYPE_NORMAL
} RPN_NODE_TYPE;

struct rpn_node {
    RPN_NODE_TYPE node_type;
    struct ast_node* exp;
    struct rpn_node* next;
    struct rpn_node* tail;
};

static struct rpn_node* secd_convert_to_rpn(struct ast_node*);
static struct rpn_node* _convert_to_rpn(struct ast_node*, struct rpn_node*);
static inline struct rpn_node* create_rpn_node();
static void rpn_dump(struct rpn_node*);
static void rpn_destroy(struct rpn_node*);

#endif