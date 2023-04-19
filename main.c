#include <stdio.h>
#include <stdbool.h>

#include "ex_calc.h"
#include "types.h"

int main()
{
    char input[] = "sin(x + 1/2 + s22)^2";
    List *result = ec_convertToRPN(input);

    ListNode *curr = result->head;
    while (curr)
    {
        printf("%s ", curr->data.value);
        curr = curr->next;
    }
    printf("\n");
    
    return 0;
}