#include "ex_calc.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#include "exception_handler.h"

const int kPrefixFuncsCount = 12;
const char kPrefixFuncs[][6] = {   "cos", "sin", "tg", "ln", "sqrt",
                                "pow", "abs", "exp", "real", "imag",
                                "mag", "phase"  };
const char kBinOpers[] = "+-*/^";

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

List *ec_convertToRPN(char expr[])
{
    List *res = lst_new();
    Stack *stack = st_new();
    
    DataNode buf;
    bool buf_use = false;
    int ptr = 0;

    bool check_unary_minus = true;

    for (int i = 0; i < strlen(expr) + 1; ++i)
    {
        char c = expr[i];

        if (c == ' ')
            continue;
        
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
                    buf.value[ptr++] = '\0'; // ебучие С строки
                    lst_pushBack(res, buf);
                    check_unary_minus = false;
                    _clearBuf(&buf, &buf_use, &ptr);
                }
                break;
            
            case TEMPORARY:
                if (isalnum(c))
                {
                    buf.value[ptr++] = c;
                } else
                {
                    buf.value[ptr++] = '\0'; // ебучие С строки
                    if (_isPrefixFunc(buf.value)) // функция
                    {
                        buf.type = PREFIX_FUNC;
                        st_push(stack, buf);
                        check_unary_minus = false;
                        _clearBuf(&buf, &buf_use, &ptr);
                    } else // переменная
                    {
                        buf.type = VARIABLE;
                        lst_pushBack(res, buf);
                        check_unary_minus = false;
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
            if (isdigit(c))
            {
                buf.type = REAL_CONSTANT;
                buf_use = true;
                buf.value[ptr++] = c;
            }

            if (isalpha(c))
            {
                buf.type = TEMPORARY;
                buf_use = true;
                buf.value[ptr++] = c;
            }

            if (c == '(')
            {
                buf.type = OPEN_BRACKET;
                check_unary_minus = true;
                st_push(stack, buf);
                _clearBuf(&buf, &buf_use, &ptr);
            }

            if (c == ')')
            {
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

            if (_isBinaryOperation(c))
            {
                if (c == '-' && check_unary_minus)
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
                        if  (temp.type == PREFIX_FUNC ||
                            (temp.type == BIN_OPERATION && _getOperationPriority(temp.value[0]) >= priority))
                        {
                            lst_pushBack(res, st_pop(stack, true));
                        } else break;
                    }
                    buf.type = BIN_OPERATION;
                    buf.value[0] = c;
                    st_push(stack, buf);
                    _clearBuf(&buf, &buf_use, &ptr);
                }
            }
        }
    }

    while (!st_isEmpty(stack))
    {
        lst_pushBack(res, st_pop(stack, true));
    }

    return res;
}