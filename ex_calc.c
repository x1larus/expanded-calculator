#include "ex_calc.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#include "exception_handler.h"


const char kBinOpers[] = "+-*/^";

// Возвращает ссылку на префиксную функцию по ее названию
// Если такой функции нет, возвращает NULL
void* _getPrefixFunc(char *val);

// Переработать
bool _isBinaryOperation(char c);

// Очищает DataNode, используется в парсере
void _clearBuf(DataNode *buf, bool *buf_use, int *ptr);

// Возвращает приоритет операции
// (чем больше, тем больше приоритет)
int _getOperationPriority(char c);


#pragma region ServiceFunctions

void* _getPrefixFunc(char *val)
{
    for (int i = 0; i < kPrefFuncsCount; ++i)
    {
        if (!strcmp(val, kPrefFuncs[i].name)) // возвращает 0, если равны
            return kPrefFuncs[i].func;
    }
    return NULL;
}

bool _isBinaryOperation(char c)
{
    for (int i = 0; i < strlen(kBinOpers); ++i)
    {
        if (c == kBinOpers[i])
            return true;
    }
    return false;
}

void _clearBuf(DataNode *buf, bool *buf_use, int *ptr)
{
    memset(buf, 0, sizeof(buf));
    *buf_use = false; 
    *ptr = 0;
}

int _getOperationPriority(char c)
{
    switch (c)
    {
    case '+':
    case '-':
        return 0;
    
    case '*':
    case '/':
        return 1;
    
    case '^':
        return 2;
    
    default:
        exception("What bin operation?..", __FUNCTION__, __FILE__, __LINE__);
    }
}

#pragma endregion ServiceFunctions

List *ec_convertToRPN(char expr[])
{
    List *res = lst_new();
    Stack *stack = st_new();
    
    DataNode buf;
    bool buf_use = false;
    int ptr = 0;

    for (int i = 0; i < strlen(expr) + 1; ++i) // Пока есть ещё символы для чтения
    {
        char c = expr[i]; // Читаем очередной символ

        if (c == ' ') continue; // Пропускаем пробелы
        
        if (buf_use)
        {
            switch (buf.type)
            {
            case REAL_CONSTANT:
                if (isdigit(c) || c == '.')
                {
                    buf.value[ptr++] = c;
                } else
                {
                    if (c == 'j')
                        buf.type = IMAGINARY_CONSTANT;
                    lst_pushBack(res, buf);
                    _clearBuf(&buf, &buf_use, &ptr);
                }
                break;
            
            case TEMPORARY:
                if (isalnum(c))
                {
                    buf.value[ptr++] = c;
                } else
                {
                    void *func = _getPrefixFunc(buf.value);
                    if (func) // функция
                    {
                        buf.type = PREFIX_FUNC;
                        buf.func = func;
                        st_push(stack, buf);
                        _clearBuf(&buf, &buf_use, &ptr);
                    } else // переменная
                    {
                        buf.type = VARIABLE;
                        lst_pushBack(res, buf);
                        _clearBuf(&buf, &buf_use, &ptr);
                    }
                }
                break;

            default:
                exception("Unsupported data type", __FUNCTION__, __FILE__, __LINE__);
                break;
            }
        }

        if (!buf_use)
        {
            if (c == '\0') // Строка кончилась
                break;

            if (isdigit(c)) // Если первый символ - число, то это точно константа
            {
                buf.type = REAL_CONSTANT;
                buf_use = true;
                buf.value[ptr++] = c;
            }

            if (isalpha(c)) // Если первый символ - буква, то это либо префиксная функция, либо переменная...
            {
                buf.type = TEMPORARY; // ...А что именно - ъуъ знает
                buf_use = true;
                buf.value[ptr++] = c;
            }

            if (c == '(') // Если символ является открывающей скобкой...
            {
                buf.type = OPEN_BRACKET;
                st_push(stack, buf); // ...помещаем его в стек
                _clearBuf(&buf, &buf_use, &ptr);
            }

            if (c == ')') // Если символ является закрывающей скобкой:
            {
                // До тех пор, пока верхним элементом стека не станет открывающая скобка,
                // выталкиваем элементы из стека в выходную строку. При этом открывающая скобка удаляется из стека,
                // но в выходную строку не добавляется. Если стек закончился раньше, чем мы встретили открывающую скобку,
                // это означает, что в выражении либо неверно поставлен разделитель, либо не согласованы скобки.
                DataNode temp;
                while (1)
                {
                    if (st_isEmpty(stack))
                        exception("Missing bracket(s)!", __FUNCTION__, __FILE__, __LINE__);

                    temp = st_pop(stack, true);
                    if (temp.type == OPEN_BRACKET)
                        break;
                    else
                        lst_pushBack(res, temp);
                }
            }

            // Если символ является бинарной операцией
            if (_isBinaryOperation(c))
            {
                if (c == '-' && 0) // ИСПРАВИТЬ СРОЧНО!!!!!!!!!!!
                {
                    buf.type = UNARY_MINUS;
                    st_push(stack, buf);
                    _clearBuf(&buf, &buf_use, &ptr);
                } else
                {
                    int priority = _getOperationPriority(c);
                    DataNode temp;
                    while (1)
                    {
                        if (st_isEmpty(stack))
                            break;
                        
                        temp = st_pop(stack, false);
                        // Пока на вершине стека префиксная функция
                        // ИЛИ
                        // операция на вершине стека приоритетнее или такого же уровня приоритета
                        if  (temp.type == PREFIX_FUNC ||
                            (temp.type == BIN_OPERATION && _getOperationPriority(temp.value[0]) >= priority))
                        {
                            lst_pushBack(res, st_pop(stack, true)); // выталкиваем верхний элемент стека в выходную строку
                        } else break;
                    }
                    buf.type = BIN_OPERATION;
                    buf.value[0] = c;
                    st_push(stack, buf); // помещаем операцию o1 в стек
                    _clearBuf(&buf, &buf_use, &ptr);
                }
            }
        }
    }

    // Когда входная строка закончилась, выталкиваем все символы из стека в выходную строку. 
    // В стеке должны были остаться только символы операций; если это не так, значит в выражении не согласованы скобки.
    while (!st_isEmpty(stack))
    {
        lst_pushBack(res, st_pop(stack, true));
    }

    return res;
}