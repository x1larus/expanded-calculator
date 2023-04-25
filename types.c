#include "types.h"
#include "exception_handler.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#pragma region StackMethods

Stack *st_new()
{
    Stack *res = (Stack*)malloc(sizeof(Stack));
    res->top = -1;
    res->data = (DataNode*)malloc(sizeof(DataNode)*MAX_STACK_SIZE);
    return res;
}

bool st_isEmpty(Stack *st)
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
    if (st_isEmpty(st))
        exception("Stack is empty", __FUNCTION__, __FILE__, __LINE__);
    if (delete)
        return st->data[(st->top)--];
    else
        return st->data[st->top];
}

#pragma endregion StackMethods

#pragma region ListMethods

List *lst_new()
{
    List *res = (List*)malloc(sizeof(List));
    res->head = NULL;
    res->tail = NULL;
    res->size = 0;
}

// Откровенный костыль
List *lst_getCopy(List *lst)
{
    List *res = (List*)malloc(sizeof(List));
    res->size = lst->size;
    ListNode *curr = lst->head;

    while (curr)
    {
        lst_pushBack(res, curr->data);
        curr = curr->next;
    }
    
    return res;
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

bool lst_isEmpty(List *lst)
{
    if (lst->head == NULL)
        return true;
    else
        return false;
}

ListNode *lst_find(List *lst, char val[])
{
    ListNode *curr = lst->head;
    if (curr == NULL)
        exception("List is empty", __FUNCTION__, __FILE__, __LINE__);
    
    while (curr)
    {
        if (strcmp(curr->data.value, val) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}

void lst_replaceInsert(List *lst, ListNode **replacedElem, List *insertedLst)
{
    if (lst_isEmpty(lst))
        exception("List is empty", __FUNCTION__, __FILE__, __LINE__);
    
    insertedLst->head->prev = (*replacedElem)->prev;
    if ((*replacedElem)->prev)
        (*replacedElem)->prev->next = insertedLst->head;
    else
        lst->head = insertedLst->head;

    insertedLst->tail->next = (*replacedElem)->next;
    if ((*replacedElem)->next)
        (*replacedElem)->next->prev = insertedLst->tail;
    else
        lst->tail = insertedLst->tail;

    lst->size += insertedLst->size - 1;

    // free(replacedElem);
}

#pragma endregion ListMethods