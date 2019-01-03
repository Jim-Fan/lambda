#ifndef _BINDING_H
#define _BINDING_H

#include "lambda.h"

struct binding {
    unsigned int var_id;
    struct node* value;
    struct binding* next;
};

void dump_binding();
void clear_binding();

struct binding* global_binding_list();
struct binding* create_binding(struct node* var, struct node* value);

#endif
