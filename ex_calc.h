#ifndef EX_CALC_H
#define EX_CALC_H

#include <complex.h>
#include "types.h"

// Возвращает лист объектов в обратной польской записи
List *ec_convertToRPN(char expr[]);

// Возвращает имена всех неизвестных переменных
// Если переменных нет, возвращает NULL
List *ec_getVariablesList(List *expr);

// Заменяет переменные в выражении на их значение
void ec_addVariablesValues(List *expr, int n, List *variables, List **variables_values);

// Считает результат выражения
double complex ec_calculate(List *expr);

#endif // EX_CALC_H