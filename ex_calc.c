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

List *ec_getVariablesList(List *expr)
{
    List *res = lst_new();
    ListNode *curr = expr->head;
    if (curr == NULL)
        exception("Expression is empty", __FUNCTION__, __FILE__, __LINE__);
    
    while (curr)
    {
        if (curr->data.type == VARIABLE && !lst_find(res, curr->data.value))
            lst_pushBack(res, curr->data);
        curr = curr->next;
    }

    return res->size ? res : NULL;
}

void ec_addVariablesValues(List *expr, int n, List *variables, List **variables_values)
{
    ListNode *curr_var = variables->head;
    for (int i = 0; i < n; i++)
    {
        for (ListNode *var_in_expr = lst_find(expr, curr_var->data.value); var_in_expr; var_in_expr = lst_find(expr, curr_var->data.value))
        {
            lst_replaceInsert(expr, &var_in_expr, variables_values[i]);
        }
        curr_var = curr_var->next;
    }
}

double complex ec_calculate(List *expr)
{
    if (lst_isEmpty(expr))
        exception("Expr is empty", __FUNCTION__, __FILE__, __LINE__);

    if (ec_getVariablesList(expr))
        exception("Some variables are undefined", __FUNCTION__, __FILE__, __LINE__);
    
    double complex st[10] = {0};
    int ptr = 0;

    for (ListNode *x = expr->head; x; x = x->next)
    {
        switch (x->data.type)
        {
        case REAL_CONSTANT:
            st[ptr++] = atof(x->data.value);
            break;
        
        case IMAGINARY_CONSTANT:
            st[ptr++] = atof(x->data.value)*I;
        
        case PREFIX_FUNC:
            if (!strcmp(x->data.value, "cos"))
                st[ptr-1] = ccos(st[ptr-1]);
            else if (!strcmp(x->data.value, "sin"))
                st[ptr-1] = csin(st[ptr-1]);
            else if (!strcmp(x->data.value, "tg"))
                st[ptr-1] = ctan(st[ptr-1]);
            else if (!strcmp(x->data.value, "ln"))
                st[ptr-1] = clog(st[ptr-1]);
            else if (!strcmp(x->data.value, "sqrt"))
                st[ptr-1] = csqrt(st[ptr-1]);
            else if (!strcmp(x->data.value, "abs"))
                st[ptr-1] = cabs(st[ptr-1]);
            else if (!strcmp(x->data.value, "exp"))
                st[ptr-1] = cexp(st[ptr-1]);
            else if (!strcmp(x->data.value, "real"))
                st[ptr-1] = creal(st[ptr-1]);
            else if (!strcmp(x->data.value, "imag"))
                st[ptr-1] = cimag(st[ptr-1]);
            else if (!strcmp(x->data.value, "~"))
                st[ptr-1] = -st[ptr-1];
            break;
        
        case BIN_OPERATION:
            if (!strcmp(x->data.value, "+"))
            {
                st[ptr-2] = st[ptr-2] + st[ptr-1];
                ptr--;
            } else if (!strcmp(x->data.value, "-"))
            {
                st[ptr-2] = st[ptr-2] - st[ptr-1];
                ptr--;
            } else if (!strcmp(x->data.value, "*"))
            {
                st[ptr-2] = st[ptr-2] * st[ptr-1];
                ptr--;
            } else if (!strcmp(x->data.value, "/"))
            {
                st[ptr-2] = st[ptr-2] / st[ptr-1];
                ptr--;
            } else if (!strcmp(x->data.value, "^"))
            {
                st[ptr-2] = cpow(st[ptr-2], st[ptr-1]);
                ptr--;
            }
            break;

        default:
            break;
        }
    }

    if (ptr != 1)
        exception("Expr syntax error", __FUNCTION__, __FILE__, __LINE__);

    return st[0];
}