#ifndef EX_CALC_H
#define EX_CALC_H

#include "types.h"


// Возвращает лист объектов в обратной польской записи
List *ec_convertToRPN(char expr[]);

// Возвращает имена всех неизвестных переменных
// Если переменных нет, возвращает NULL
List *ec_getVariablesList(List *expr);

// Заменяет переменные в выражении на их значение
void ec_addVariablesValues(List *expr, int n, List *variables, List **variables_values);

#endif // EX_CALC_H