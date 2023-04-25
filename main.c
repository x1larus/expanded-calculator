#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "ex_calc.h"

#define MAX_INPUT 100

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

    char *varName = ec_getVariable(result);
    while (varName)
    {
        printf("%s = ", varName);
        scanf("%s", input);
        ec_addVariableValue(result, varName, input);
        print_expr(result);
        varName = ec_getVariable(result);
    }
    

    print_expr(result);
    
    return 0;
}