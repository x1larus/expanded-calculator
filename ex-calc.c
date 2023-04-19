#include "ex-calc.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>


List ec_convert_to_postfix(char infix_expr[])
{
    //node *ans = (node*)malloc(sizeof(node)*MAX_NODES_COUNT); // результирующая "строка"
    int ans_ptr = 0; // указатель на текущий элемент результирующей строки, одновременно ее размер
    //memset(ans, 0, sizeof(node)*MAX_NODES_COUNT); // занулить все
    Stack *st = st_new(); // создание стека

    ValueType buf_type; // тип данных в буффере
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