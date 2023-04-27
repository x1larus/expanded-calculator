#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define MAX_NODE_VALUE_LEN 10
#define MAX_STACK_SIZE 20


// Констатный справочник префиксных функций
static const char kPrefixFuncs[][10] = {"cos", "sin", "tg", "ln", "sqrt", "abs", "exp", "~", "real", "imag", "mag", "phase"}; // куда ж без ебучего унарного минуса
static const int kPrefixFuncsCount = 12;

// Констатный справочник бинарных операций
static const char kBinaryOperators[] = "+-*/^";

// Тип ноды с данными
typedef enum value_type
{
    VARIABLE,
    REAL_CONSTANT,
    IMAGINARY_CONSTANT,
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
} DataNode;

// Стек и стек, че бубнить то
typedef struct stack
{
    int top;
    DataNode *data;
} Stack;

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

#pragma region StackMethods

// Создает стек
Stack *st_new();

// Возвращает true, если стек пуст
bool st_isEmpty(Stack *st);

// Добавляет элемент в стек
void st_push(Stack *st, DataNode val);

// Возвращает верхний элемент стека
// При delete=false, элемент не будет удален
DataNode st_pop(Stack *st, bool delete);

#pragma endregion StackMethods

#pragma region ListMethods

// Создает новый лист
List *lst_new();

// Возвращает копию листа в новой памяти
List *lst_getCopy(List *lst);

// Добавляет новый элемент в конец списка
void lst_pushBack(List *lst, DataNode val);

// Возвращает true, если лист пуст
bool lst_isEmpty(List *lst);

// Возвращает листноду, если listnode.data.value == val
// Если таковой не найдено - NULL
ListNode *lst_find(List *lst, char val[]);

// Скрещение функций replace и insert
// Параметры: где менять, какой именно менять, на что менять
void lst_replaceInsert(List *lst, ListNode **replacedElem, List *insertedLst);

// Добавляет из src в конец dest те объекты типа VARIABLE, которых нет в dest
void lst_addUnique(List *dest, List *src);

#pragma endregion ListMethods

#endif // TYPES_H