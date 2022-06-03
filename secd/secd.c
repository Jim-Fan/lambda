#include "secd.h"
#include <malloc.h>
#include <stdio.h>

//////////////////////////////     SECD     //////////////////////////////
void secd_init()
{
    // Stub
}

void secd_destroy()
{
    // Stub
}

void secd_eval(struct ast_node* ast)
{
    struct rpn_node* rpn = secd_convert_to_rpn(ast);
    rpn_dump(rpn);
    rpn_destroy(rpn);
    rpn = NULL;
}

//////////////////////////////     RPN     //////////////////////////////
static struct rpn_node* secd_convert_to_rpn(struct ast_node* root)
{
    // struct rpn_node* head = (struct rpn_node*) malloc(sizeof(struct rpn_node));
    // struct rpn_node* tail = head;
    return _convert_to_rpn(root, (struct rpn_node*) NULL);
}

static struct rpn_node* _convert_to_rpn(struct ast_node* ast, struct rpn_node* head)
{
    if (ast == NULL) return NULL;

    // Same for lambda expression?
    if (ast->node_type == NODE_TYPE_NUMBER || ast->node_type == NODE_TYPE_VAR || ast->node_type == NODE_TYPE_LAMBDA)
    {
        if (head == NULL)
        {
            head = create_rpn_node();
            head->node_type = RPN_NODE_TYPE_NORMAL;
            head->exp = ast;
            head->next = NULL;
            head->tail = head;
        }
        else
        {
            struct rpn_node* node = create_rpn_node();
            node->node_type = RPN_NODE_TYPE_NORMAL;
            node->exp = ast;

            head->tail->next = node;
            head->tail = node;
        }

        return head;
    }
    
    // Application F x  =>  x:F:ap
    else if (ast->node_type == NODE_TYPE_APP)
    {
        struct rpn_node* new_head = _convert_to_rpn(ast->right, head);
        new_head = _convert_to_rpn(ast->left, new_head);

        struct rpn_node* ap = create_rpn_node();
        ap->node_type = RPN_NODE_TYPE_AP;
        ap->exp = NULL;
        ap->next = NULL;

        new_head->tail->next = ap;
        new_head->tail = ap;
        return new_head;
    }
    
    return NULL;
}

static inline struct rpn_node* create_rpn_node()
{
    return (struct rpn_node*) malloc(sizeof(struct rpn_node));
}

static void rpn_dump(struct rpn_node* rpn)
{
    printf("<rpn>\n");

    struct rpn_node* ptr = rpn;
    while (ptr != NULL)
    {
        if (ptr->node_type == RPN_NODE_TYPE_AP)
        {
            printf("| AP\n");
        }
        else if (ptr->node_type == RPN_NODE_TYPE_NORMAL)
        {
            struct ast_node* exp = ptr->exp;
            if (exp->node_type == NODE_TYPE_VAR)
            {
                printf("| VAR(%d) %c\n", exp->var_id, exp->var_name);
            }
            else if (exp->node_type == NODE_TYPE_LAMBDA)
            {
                printf("| LAMBDA(%d)\n", exp->var_id);
            }
            else if (exp->node_type == NODE_TYPE_APP)
            {
                printf("| APP(%d)\n", exp->var_id);
            }
            else if (exp->node_type == NODE_TYPE_NUMBER)
            {
                printf("| NUMBER(%d) %d\n", exp->var_id, exp->num_value);
            }
        }

        ptr = ptr->next;
    }
    printf("\n");
}

static void rpn_destroy(struct rpn_node* rpn)
{
    struct rpn_node* ptr = rpn;
    while (ptr != NULL) {
        rpn = rpn->next;
        free(ptr);
        ptr = rpn;
    }
}