#include <stdio.h>
#include "exprparser.h"
int main()
{
    char *a;
    double b;
    scanf("%s",a);
    scanf("%f",&b);
    printf("%f",expr_parse(a,b));
}