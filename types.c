#include "types.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "exception_handler.h"


Stack *st_new()
{
    Stack *res = (Stack*)malloc(sizeof(Stack));
    res->top = -1;
    res->data = (DataNode*)malloc(sizeof(DataNode)*MAX_STACK_SIZE);
    return res;
}

bool st_is_empty(Stack *st)
{
    return st->top == -1 ? true : false;
}

void st_push(Stack *st, DataNode val)
{
    if (st->top == MAX_STACK_SIZE - 1)
        exception("Stack overflow", __FUNCTION__, __FILE__, __LINE__);
    st->data[++(st->top)] = val;
}

DataNode st_pop(Stack *st, bool delete)
{
    if (st_is_empty(st))
        exception("Stack is empty", __FUNCTION__, __FILE__, __LINE__);
    if (!delete)
        return st->data[(st->top)--];
    else
        return st->data[st->top];
}

List *lst_new()
{
    List *res = (List*)malloc(sizeof(List));
    res->head = NULL;
    res->tail = NULL;
    res->size = 0;
}

void lst_pushBack(List *lst, DataNode val)
{
    ListNode *new_node = (ListNode*)malloc(sizeof(ListNode));
    new_node->prev = NULL;
    new_node->next = NULL;
    new_node->data = val;

    if (lst->head && lst->tail)
    {
        new_node->prev = lst->tail;
        lst->tail->next = new_node;
        lst->tail = new_node;
        lst->size += 1;
    } else
    {
        lst->head = new_node;
        lst->tail = new_node;
        lst->size += 1;
    }
    return;
}
