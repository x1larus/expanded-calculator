#include "ex_calc.h"
#include "exception_handler.h"
#include "types.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


// Возвращает true если такая префиксная функция есть
bool _isPrefixFunc(char *val);

// Возвращает true если такая операция есть
bool _isBinaryOperation(char c);

// Очищает DataNode, используется в парсере
void _clearBuf(DataNode *buf, bool *buf_use, int *ptr);

// Возвращает приоритет операции
// (чем больше, тем больше приоритет)
int _getOperationPriority(char c);


#pragma region ServiceFunctions

bool _isPrefixFunc(char *val)
{
    for (int i = 0; i < kPrefixFuncsCount; ++i)
    {
        if (!strcmp(val, kPrefixFuncs[i])) // возвращает 0, если равны
            return true;
    }
    return false;
}

bool _isBinaryOperation(char c)
{
    for (long unsigned int i = 0; i < strlen(kBinaryOperators); ++i)
    {
        if (c == kBinaryOperators[i])
            return true;
    }
    return false;
}

void _clearBuf(DataNode *buf, bool *buf_use, int *ptr)
{
    memset(buf->value, 0, sizeof(buf->value));
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
    _clearBuf(&buf, &buf_use, &ptr);

    for (long unsigned int i = 0; i < strlen(expr) + 1; ++i) // Пока есть ещё символы для чтения
    {
        if (expr[i] == ' ') continue; // Пропускаем пробелы
        
        if (buf_use)
        {
            switch (buf.type)
            {
            case REAL_CONSTANT: // Если символ является числом...
                if (isdigit(expr[i]) || expr[i] == '.')
                {
                    buf.value[ptr++] = expr[i];
                } else
                {
                    if (expr[i] == 'j')
                    {
                        buf.type = IMAGINARY_CONSTANT;
                        i++; // чтобы еще раз ее не прочитать
                    }
                    lst_pushBack(res, buf); // ..то добавляем его к выходной строке
                    _clearBuf(&buf, &buf_use, &ptr);
                }
                break;
            
            case TEMPORARY:
                if (isalnum(expr[i]))
                {
                    buf.value[ptr++] = expr[i];
                } else
                {
                    if (_isPrefixFunc(buf.value)) // функция
                    {
                        buf.type = PREFIX_FUNC;
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
            if (expr[i] == '\0') // Строка кончилась
                break;

            if (expr[i] == 'j') // просто мнимая единица
            {
                buf.type = IMAGINARY_CONSTANT;
                buf.value[0] = '1';
                i++; // чтобы еще раз ее не прочитать
                lst_pushBack(res, buf);
                _clearBuf(&buf, &buf_use, &ptr);
            }

            if (isdigit(expr[i])) // Если первый символ - число, то это точно константа
            {
                buf.type = REAL_CONSTANT;
                buf_use = true;
                buf.value[ptr++] = expr[i];
            }

            if (isalpha(expr[i])) // Если первый символ - буква, то это либо префиксная функция, либо переменная...
            {
                buf.type = TEMPORARY; // ...А что именно - ъуъ знает
                buf_use = true;
                buf.value[ptr++] = expr[i];
            }

            if (expr[i] == '(') // Если символ является открывающей скобкой...
            {
                buf.type = OPEN_BRACKET;
                st_push(stack, buf); // ...помещаем его в стек
                _clearBuf(&buf, &buf_use, &ptr);
            }

            if (expr[i] == ')') // Если символ является закрывающей скобкой:
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
            if (_isBinaryOperation(expr[i]))
            {
                // ЕБАНЫЙ унарный минус может возникнуть только в двух случаях:
                // 1. После скобки
                // 2. В самом начале выражения
                if (expr[i] == '-' && (((!st_isEmpty(stack) && st_pop(stack, false).type == OPEN_BRACKET) || lst_isEmpty(res))))
                {
                    buf.type = PREFIX_FUNC; // бинарная операция, бля буду
                    buf.value[0] = '~';
                    st_push(stack, buf);
                    _clearBuf(&buf, &buf_use, &ptr);
                } else
                {
                    int priority = _getOperationPriority(expr[i]);
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
                    buf.value[0] = expr[i];
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

char *ec_getVariable(List *expr)
{
    ListNode *curr = expr->head;
    if (curr == NULL)
        exception("List is empty", __FUNCTION__, __FILE__, __LINE__);
    
    while (curr)
    {
        if (curr->data.type == VARIABLE)
            return curr->data.value;
        curr = curr->next;
    }

    return NULL;
}

int ec_addVariableValue(List *expr, char *varName, char *varValue)
{
    List *new_var = ec_convertToRPN(varValue);
    if (lst_isEmpty(new_var))
        exception("Variable value is empty", __FUNCTION__, __FILE__, __LINE__);
    
    ListNode *var_in_expr = lst_find(expr, varName);
    while (var_in_expr)
    {
        lst_replaceInsert(expr, var_in_expr, lst_getCopy(new_var)); // костыль
        var_in_expr = lst_find(expr, varName);
    }
}