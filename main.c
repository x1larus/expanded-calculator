#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "ex_calc.h"

#define MAX_INPUT 100
#define MAX_VARS_COUNT 10

void print_expr(List *expr)
{
    printf("--------------------------------------------\n");
    ListNode *curr = expr->head;
    while (curr)
    {
        printf("%s type=%d\n", curr->data.value, curr->data.type);
        curr = curr->next;
    }
    printf("--------------------------------------------\n");
}

int main()
{
    char input[MAX_INPUT];
    scanf("%s", input);
    List *result = ec_convertToRPN(input);
    print_expr(result);


    // Сборка переменных
    List *variables = ec_getVariablesList(result);
    List **variables_values = (List**)malloc(sizeof(List*)*MAX_VARS_COUNT);
    int variables_values_ptr = 0;

    for (ListNode *x = variables->head; x; x = x->next)
    {
        printf("%s = ", x->data.value);
        scanf("%s", input);
        List *variable_value = ec_convertToRPN(input);
        variables_values[variables_values_ptr++] = variable_value;
        lst_addUnique(variables, variable_value);  
    }
    ec_addVariablesValues(result, variables_values_ptr, variables, variables_values); // добавление их в выражение
    
    print_expr(result);

    
    
    return 0;
}