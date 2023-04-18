#include <stdio.h>
#include <stdbool.h>

#include "types.h"

int main()
{
    List *lst = lst_new();
    DataNode data1;
    data1.type = 0;
    DataNode data2;
    data2.type = 1;
    lst_pushBack(lst, data1);
    lst_pushBack(lst, data2);
    return 0;
}