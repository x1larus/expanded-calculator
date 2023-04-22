#ifndef EX_CALC_H
#define EX_CALC_H

#include "types.h"


// Возвращает лист объектов в обратной польской записи
List *ec_convertToRPN(char expr[]);

#endif // EX_CALC_H