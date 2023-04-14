#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#include "ex-calc.h"

#pragma region ExeptionHandler

void exeption(const char comment[], const char* func, const char *file, int line)
{
    fprintf(stderr, "In function %s() (%s:%d): %s\n", func, file, line, comment);
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
        exeption("Stack overflow", __FUNCTION__, __FILE__, __LINE__);
    st->data[++(st->top)] = val;
}

// Возвращает верхний элемент стека
// При delete=true, элемент не будет удален
node st_pop(stack *st, bool delete)
{
    if (st_is_empty(st))
        exeption("Stack is empty", __FUNCTION__, __FILE__, __LINE__);
    if (delete)
        return st->data[(st->top)--];
    else
        return st->data[st->top];
}

#pragma endregion Stack

node* ec_convert_to_postfix(char infix_expr[])
{
    node *ans = (node*)malloc(sizeof(node)*MAX_NODES_COUNT); // результирующая "строка"
    int ans_ptr = 0; // указатель на текущий элемент результирующей строки, одновременно ее размер
    memset(ans, 0, sizeof(node)*MAX_NODES_COUNT); // занулить все
    stack *st = st_new(); // создание стека

    value_type buf_type; // тип данных в буффере
    char buf[MAX_NODE_VALUE_LEN]; // буффер, в котором будут парситься числа, функции итд
    int buf_ptr = 0; // указатель на текущий элемент буффера, одновременно его размер
    memset(buf, 0, MAX_NODE_VALUE_LEN); // очистить буффер
    bool buf_use = false; // флаг использования буффера

    bool write_result_flag = false; // флаг необходимости записи в результирующюю строку
    bool add_stack_flag = false; // флаг необходимости добавления в стек
    
    for (int i = 0; infix_expr[i] != 0; ++i)
    {
        if (infix_expr[i] == ' ') 
            continue;

        if (buf_use) // если что то в буффер пишется
        {
            switch (buf_type)
            {
            case REAL_CONSTANT:
                if (isdigit(infix_expr[i]) || infix_expr[i] == '.')
                    buf[buf_ptr++] = infix_expr[i];
                else if (infix_expr[i] == 'j')
                    buf_type = IMAGINARY_CONSTANT;
                    write_result_flag = true;
                break;
            
            default:
                break;
            }
        } else
        {
            if (isdigit(infix_expr[i]))
            {
                buf[buf_ptr++] = infix_expr[i];
                buf_type = REAL_CONSTANT;
                continue;
            }
        }

        if (write_result_flag)
        {
            ans[ans_ptr].type = buf_type;
            strncpy(ans[ans_ptr].value, buf, buf_ptr);
            ans_ptr++;
            write_result_flag = false;
            memset(buf, 0, MAX_NODE_VALUE_LEN); // очистить буффер
        }
    }
}