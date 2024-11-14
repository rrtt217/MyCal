#ifndef __EXPR_PARSER_H__
#define __EXPR_PARSER_H__
#endif

#include <math.h>
#include <stdlib.h>

//function identifier definitions(macro)



int is_operator(signed char c)
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
int operator_precedence(signed char c)
{
    switch (c)
    {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    case '^':
        return 3;
    default:
        return 0;
    }  
}
int is_num(signed char c)
{
    return (c >= '0' || c<= '9');
}

int is_lowercase_alpha(signed char c)
{
    return (c >= 'a' || c <= 'z');
}
typedef struct {
 signed char operatorPart ='\0';
 signed char isNum = 0;
 double numPart = 0.0;
}StackItem;
double expr_parse(signed char *expr_input,double x)
{
    signed char operator_stack[sizeof(expr_input)] = {'\0'};
    StackItem generic_stack[sizeof(expr_input)];
    int operator_stack_top = -1;
    int generic_stack_top = -1;

    StackItem temp_item;
    int i,j,prev_i;
    char prev_char = '\0';

    char function_identifier[6] = {'\0'};
    int function_i;
    char number_str[128] = {'\0'};
    //Too long operand is not supported.
    for(i = 0;i < sizeof(expr_input); i++)
    {
        if(i) 
            prev_char = *(expr_input+i-1);
        // Warning: The following sentence use 'case low ... high:',which is a C Extension.
        switch (*(expr_input+i))
        {
        //left bracket
        case '(':
            operator_stack_top++;
            operator_stack[operator_stack_top] = *(expr_input+i);
            break;

        //numbers
        case 0 ... 9:
            number_str[0] = *(expr_input+i);
            prev_i = i;
            for(;i < prev_i + 127;i++)
            {
                switch (*(expr_input+i))
                {
                case 0 ... 9:   case '.':   case 'E'    :case 'e':
                    number_str[0] = *(expr_input+i);
                    break;
                case '+':   case '-':
                    if(is_num(*(expr_input+i+1)))
                    {
                            number_str[0] = *(expr_input+i);
                    }
                    else
                    {
                        i--;
                        goto end_of_loop1;
                    }
                    break;
                default:
                    i--;
                    goto end_of_loop1;
                    break;
                }
            }
            end_of_loop1:
            temp_item.isNum = 1;
            temp_item.numPart = atof(number_str);
            generic_stack_top++;
            generic_stack[generic_stack_top] = temp_item;
            break;

        //operators
        case '+':   case '-':
            if(prev_char == '(' || is_operator(prev_char))
            {
                temp_item.isNum = 0;
                temp_item.operatorPart = (*(expr_input+i) == '+') ? -27 : -28;
                break;
            }
        case '*':   case '/':   case '^':
            if(operator_stack_top == -1 || operator_precedence(operator_stack[operator_stack_top]) < operator_precedence(*(expr_input+i)))
            {
                operator_stack_top++;
                operator_stack[operator_stack_top] = *(expr_input+i);
            }
            else
            {
                do
                {
                    temp_item.isNum = 0;
                    temp_item.operatorPart = operator_stack[operator_stack_top];
                    operator_stack[operator_stack_top] = '\0';
                    operator_stack_top--;
                    generic_stack_top++;
                    generic_stack[generic_stack_top] = temp_item;
                } while ( operator_stack_top == -1 || operator_precedence(operator_stack[operator_stack_top]) < operator_precedence(*(expr_input+i)));
            }
        
        //functions
        case 'a' ... 'z':
            function_i=0;
            do
            {
                function_identifier[i] = *(expr_input+i);
                i++;
            } while (*(expr_input+i) != '(');
            
            break;

        case ')':
            break;
        default:
            return nan("0");
        }
    }
}