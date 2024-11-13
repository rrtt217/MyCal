#ifndef __EXPR_PARSER_H__
#define __EXPR_PARSER_H__
#endif

#include <math.h>

int is_operator(char c)
{
    switch (c)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '^':
        return 1;
        break;
    default:
        return 0;
        break;
    }
}
int is_num(char c)
{
    return (c >= '0' || c<= '9');
}
typedef struct {
 char operatorPart ='\0';
 char isNum = 0;
 double numPart = 0.0;
}StackItem;
double expr_parse(char expr_input[],double x)
{
    char operator_stack[sizeof(expr_input)] = {'\0'};
    StackItem generic_stack[sizeof(expr_input)];
    int i,j;
    int operator_stack_top = -1;
    int generic_stack_top = -1;
    for(i = 0;i < sizeof(expr_input); i++)
    {

    }
}