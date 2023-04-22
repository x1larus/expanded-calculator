#pragma once

#include <stdbool.h>

#define MAX_NODE_VALUE_LEN 10
#define MAX_STACK_SIZE 20


// Тип ноды с данными
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

// Основная нода
typedef struct data_node
{
    ValueType type; // тип данных
    char value[MAX_NODE_VALUE_LEN]; // значение
    void *func; // ссылка на функцию, если это операция
} DataNode;

// Стек и стек, че бубнить то
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

// Нода двусвязного списка
typedef struct list_node
{
    struct list_node *prev;
    struct list_node *next;
    DataNode data;
} ListNode;

// Лист и лист, че бубнить то
typedef struct list
{
    ListNode *head;
    ListNode *tail;
    int size;
} List;

// Создает новый лист
List *lst_new();

// Добавляет новый элемент в конец списка
void lst_pushBack(List *lst, DataNode val);