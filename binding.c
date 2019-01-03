#include "binding.h"

static struct binding* BINDING_HEAD = NULL;

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
        if (tmp->value != NULL) free_node(tmp->value);
        tmp->value = NULL;
        free(tmp);
        tmp = b;
    }
    BINDING_HEAD = NULL;
}

struct binding* global_binding_list()
{
    return BINDING_HEAD;
}

struct binding* create_binding(struct node* var, struct node* value)
{
    struct binding* b = BINDING_HEAD;
    BINDING_HEAD = (struct binding*) malloc(sizeof(struct binding));
    BINDING_HEAD->next = b;
    BINDING_HEAD->var_id = var->var_id;

    // Value of binding now points to new discrete structure, no more loop
    BINDING_HEAD->value = node_deep_copy(value);
    return BINDING_HEAD;
}
