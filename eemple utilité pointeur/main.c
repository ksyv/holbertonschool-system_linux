#include <stdio.h>
#include <stdlib.h>
#include "main.h"


int main(void)
{
    int x, y, z;
    x = 10;
    y = 20;
    z = x + y;
    printf("%d \n", z);
    second(&y);
    return (0);
}
void second(int *y)
{
    printf("%d \n", *y);
}

