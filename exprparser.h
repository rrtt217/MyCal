#ifndef __EXPR_PARSER_H__
#define __EXPR_PARSER_H__
#endif

#include <math.h>
double expr_parse(char expr_input[],double x)
{
    char expr[1024] = {'\0'};
    char stack[1024];
    int i;
    if(sizeof(expr_input) > 1024)
        return _HUGE_ENUF*_HUGE_ENUF*0.0;
    else
    {
        for(i = 0;i < 1024; i++)
        {
            expr[i]=expr_input[i];                  //copy the original expression to avoid issues
        }

    }
}