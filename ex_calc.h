#ifndef EX_CALC_H
#define EX_CALC_H

#include "types.h"


// Возвращает лист объектов в обратной польской записи
List *ec_convertToRPN(char expr[]);

// Возвращает первую переменную в выражении
// Если переменных нет, возвращает NULL
char *ec_getVariable(List *expr);

// Заменяет переменную в выражении на ее значение
// Возвращает количество замен
int ec_addVariableValue(List *expr, char *varName, char *varValue);

#endif // EX_CALC_H