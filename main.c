#include <stdio.h>
#include <stdbool.h>
#include "ex_calc.h"

int main()
{
    char input[] = "-exp(1/2j+x+ser1234)";
    List *result = ec_convertToRPN(input);

    ListNode *curr = result->head;
    while (curr)
    {
        printf("%s type=%d\n", curr->data.value, curr->data.type);
        curr = curr->next;
    }
    printf("\n");
    
    return 0;
}