#include <math.h>
#include <stdlib.h>
#include <string.h>
//function identifier definitions(macro)

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
int operator_precedence(char c)
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
int is_num(char c)
{
    return (c >= '0' || c<= '9');
}

int is_lowercase_alpha(char c)
{
    return (c >= 'a' || c <= 'z');
}

typedef struct {
 char operatorPart;
 char isNum;
 double numPart;
}StackItem;

StackItem empty_item = {'0',0,0.0};

int is_empty(StackItem item)
{
    return (item.isNum == 0 && item.numPart == 0.0 && item.operatorPart == '\0');
}

//Convert function to char that represents a monocular operator. The mapping can be found in helper.txt.
char function_wrapper(char* function_identifier)
{
    if(!strcmp(function_identifier,"abs"))
        return -1;
    else if(!strcmp(function_identifier,"exp"))
        return -2;
    else if(!strcmp(function_identifier,"ln"))
        return -3;
    else if(!strcmp(function_identifier,"lg"))
        return -4;
    else if(!strcmp(function_identifier,"sqrt"))
        return -5;
    else if(!strcmp(function_identifier,"cbrt"))
        return -6;
    else if(!strcmp(function_identifier,"sin"))
        return -7;
    else if(!strcmp(function_identifier,"cos"))
        return -8;
    else if(!strcmp(function_identifier,"tan"))
        return -9;
    else if(!strcmp(function_identifier,"asin"))
        return -10;
    else if(!strcmp(function_identifier,"acos"))
        return -11;
    else if(!strcmp(function_identifier,"atan"))
        return -12;
    else if(!strcmp(function_identifier,"sinh"))
        return -13;
    else if(!strcmp(function_identifier,"cosh"))
        return -14;
    else if(!strcmp(function_identifier,"tanh"))
        return -15;
    else if(!strcmp(function_identifier,"asinh"))
        return -16;
    else if(!strcmp(function_identifier,"acosh"))
        return -17;
    else if(!strcmp(function_identifier,"atanh"))
        return -18;
    else if(!strcmp(function_identifier,"erf"))
        return -19;
    else if(!strcmp(function_identifier,"erfc"))
        return -20;
    else if(!strcmp(function_identifier,"tgamma"))
        return -21;
    else if(!strcmp(function_identifier,"lgamma"))
        return -22;
    else if(!strcmp(function_identifier,"celi"))
        return -23;
    else if(!strcmp(function_identifier,"floor"))
        return -24;
    else if(!strcmp(function_identifier,"trunc"))
        return -25;
    else if(!strcmp(function_identifier,"round"))
        return -26;
    else
        return 0;
}

double expr_parse(char *expr_input,double x)
{
    char operator_stack[sizeof(expr_input)] = {'\0'};
    StackItem generic_stack[sizeof(expr_input)] = {empty_item};
    int operator_stack_top = -1;
    int generic_stack_top = -1;

    StackItem temp_item;
    int i,j,prev_i;
    char prev_char = '\0';

    char function_identifier[7] = {'\0'};
    int function_i;
    int function_brackets = 0;
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
            if(function_i == -1)
                function_brackets++;
            break;

        //numbers
        case '0' ... '9':
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
                    //i--;
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
            if(operator_stack_top == -1)
            {
                operator_stack_top++;
                operator_stack[operator_stack_top] = *(expr_input+i);
            }
            else if(operator_precedence(operator_stack[operator_stack_top]) < operator_precedence(*(expr_input+i)))
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
                } while ( operator_stack_top == -1 || operator_precedence(operator_stack[(operator_stack_top >= 0)? operator_stack_top : 0]) < operator_precedence(*(expr_input+i)));
            }
        break;
        //functions
        case 'a' ... 'z':
        if(function_i == -1)
        {
            //Empty the string after last search.
            for(function_i=0;function_i<7;function_i++)
            {
                function_identifier[function_i] = '\0';
            }  
        }
            function_i=0;
            do
            {
                function_identifier[function_i] = *(expr_input+i);
                function_i++;
                i++;
            } while (!is_lowercase_alpha(*(expr_input+i)));
        if(function_identifier[1] == '\0')
        {
            if(function_identifier[0] == 'x')
                {
                    temp_item.isNum = 1;
                    temp_item.numPart = x;
                    generic_stack_top++;
                    generic_stack[generic_stack_top] = temp_item;
                }
            else
            {
                return nan("0");
            }
        }
        else
        {
            function_i=-1;  //This represents "start searching for matched brackets".
            i--;
        }
            break;

        case ')':
            do
            {
                temp_item.isNum = 0;
                temp_item.operatorPart = operator_stack[operator_stack_top];
                operator_stack[operator_stack_top] = '\0';
                operator_stack_top--;
                generic_stack_top++;
                generic_stack[generic_stack_top] = temp_item;
            } while (operator_stack[operator_stack_top] != '(');
            operator_stack[operator_stack_top] = '\0';
            operator_stack_top--;
            if(function_i == -1)
                function_brackets--;
            if(function_brackets == 0)
                {
                    operator_stack_top++;
                    operator_stack[operator_stack_top] = function_wrapper(function_identifier);
                }
            break;
        case '\0':
            goto end_of_loop2;
        default:
            return nan("0");
        }
    }
    end_of_loop2:
    for(;operator_stack_top > -1;operator_stack_top--)
    {
        temp_item.isNum = 0;
        temp_item.operatorPart = operator_stack[operator_stack_top];
        operator_stack[operator_stack_top] = '\0';
        generic_stack_top++;
        generic_stack[generic_stack_top] = temp_item;
    }
        StackItem calculation_stack[sizeof(expr_input)] = {empty_item};
        int calculation_stack_top = -1;
        for(i = 0;!(is_empty(generic_stack[i]));i++)
        {
            if(generic_stack[i].isNum)
            {
                calculation_stack_top++;
                calculation_stack[calculation_stack_top] =generic_stack[i];
            }
            else
            {
                if(generic_stack[i].operatorPart > 0)
                {
                    switch (generic_stack[i].operatorPart)
                    {
                    case '+':
                        calculation_stack[calculation_stack_top-1].numPart += calculation_stack[calculation_stack_top].numPart;
                        calculation_stack[calculation_stack_top] = empty_item;
                        calculation_stack_top--;
                        break;
                    case '-':
                        calculation_stack[calculation_stack_top-1].numPart -= calculation_stack[calculation_stack_top].numPart;
                        calculation_stack[calculation_stack_top] = empty_item;
                        calculation_stack_top--;
                        break;
                    case '*':
                        calculation_stack[calculation_stack_top-1].numPart *= calculation_stack[calculation_stack_top].numPart;
                        calculation_stack[calculation_stack_top] = empty_item;
                        calculation_stack_top--;
                        break;
                    case '/':
                        calculation_stack[calculation_stack_top-1].numPart /= calculation_stack[calculation_stack_top].numPart;
                        calculation_stack[calculation_stack_top] = empty_item;
                        calculation_stack_top--;
                        break;
                    case '^':
                        calculation_stack[calculation_stack_top-1].numPart = pow(calculation_stack[calculation_stack_top-1].numPart,calculation_stack[calculation_stack_top].numPart);
                        calculation_stack[calculation_stack_top] = empty_item;
                        calculation_stack_top--;
                        break;        
                    default:
                        break;
                    }
                }
                else
                {
                    calculation_stack[calculation_stack_top] = calculation_stack[calculation_stack_top];
                }
            }
        }
        return calculation_stack[0].numPart;
}