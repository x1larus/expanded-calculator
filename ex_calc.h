#pragma once

#include <math.h>

#include "types.h"

// Нода для префиксных функций
typedef struct pref_funcs_node
{
    char name[10];
    double (*func)(double); // предпологается, что функции унарны (в тз бинарных префиксных нет)
} PrefFuncsNode;

// Констатный справочник префиксных функций
const PrefFuncsNode kPrefFuncs[] = {{"cos", &cos},
                                    {"sin", &sin},
                                    {"tg", &tan},
                                    {"ln", &log},
                                    {"sqrt", &sqrt},
                                    {"abs", &fabs},
                                    {"exp", &exp}};
const int kPrefFuncsCount = 7;


// Возвращает лист объектов в обратной польской записи
List *ec_convertToRPN(char expr[]);