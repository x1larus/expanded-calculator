#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#include "ex-calc.h"

#pragma region ExeptionHandler

void exeption(char comment[])
{
    fprintf(stderr, comment);
    exit(EXIT_FAILURE);
}

#pragma endregion ExeptionHandler

#pragma region Stack // базовая реализация стека

typedef struct
{
    int top;
    node *data;
} stack;

stack *st_new()
{
    stack *res = (stack*)malloc(sizeof(stack));
    res->top = -1;
    res->data = (node*)malloc(sizeof(node)*MAX_STACK_SIZE);
    return res;
}

bool st_is_empty(stack *st)
{
    return st->top == -1 ? true : false;
}

void st_push(stack *st, node val)
{
    if (st->top == MAX_STACK_SIZE - 1)
        exeption("In st_push: stack overflow");
    st->data[++(st->top)] = val;
}

node st_pop(stack *st)
{
    if (st_is_empty(st))
        exeption("In st_pop: stack is empty");
    return st->data[(st->top)--];
}

#pragma endregion Stack

node* ec_convert_to_postfix(char infix_expr[])
{
    node *ans = (node*)malloc(sizeof(node)*MAX_NODES_COUNT); // результирующая "строка"
    int ans_ptr = 0;
    memset(ans, 0, sizeof(node)*MAX_NODES_COUNT);
    stack *st = st_new();

    char buf[MAX_NODE_VALUE_LEN];
    memset(buf, 0, MAX_NODE_VALUE_LEN);
    
    for (int i = 0; infix_expr[i] != 0; i++)
    {
        if (infix_expr[i] == ' ') 
            continue;
        
        if ((!isdigit(infix_expr[i]) || infix_expr[i] != '.' ) && strlen(buf) != 0) // проверка на конец константы
        {
            ans[ans_ptr].type = CONSTANT;
            strcpy(ans[ans_ptr].value, buf);
            memset(buf, 0, MAX_NODE_VALUE_LEN);
            ans_ptr++;
            continue;
        }

        // надо дописать )

        
    }
}