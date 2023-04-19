#pragma once

#include <stdbool.h>

#define MAX_NODE_VALUE_LEN 10
#define MAX_STACK_SIZE 20

typedef enum value_type
{
    VARIABLE,
    REAL_CONSTANT,
    IMAGINARY_CONSTANT,
    UNARY_MINUS,
    PREFIX_FUNC, // like a sin ( sin(x) )
    BIN_OPERATION, // 2+2, 2*2 etc.
    OPEN_BRACKET, // (
    TEMPORARY // for parsing
} ValueType;

typedef struct data_node
{
    ValueType type;
    char value[MAX_NODE_VALUE_LEN];
} DataNode;

typedef struct
{
    int top;
    DataNode *data;
} Stack;

// Создает стек
Stack *st_new();

// Возвращает true, если стек пуст
bool st_isEmpty(Stack *st);

// Добавляет элемент в стек
void st_push(Stack *st, DataNode val);

// Возвращает верхний элемент стека
// При delete=false, элемент не будет удален
DataNode st_pop(Stack *st, bool delete);

typedef struct list_node
{
    struct list_node *prev;
    struct list_node *next;
    DataNode data;
} ListNode;

typedef struct list
{
    ListNode *head;
    ListNode *tail;
    int size;
} List;

List *lst_new();

void lst_pushBack(List *lst, DataNode val);