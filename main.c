#include <stdio.h>
#include "exprparser.h"
int main()
{
    char a[1024] = {'\0'};
    double b;
    scanf("%s",a);
    scanf("%lF",&b);
    printf("%f",expr_parse(a,1024,b));
}