#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "exprparser.h"

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
    return (c >= '0' && c<= '9');
}

int is_lowercase_alpha(char c)
{
    return (c >= 'a' && c <= 'z');
}



const static StackItem empty_item = {'\0',0,0.0};

int is_empty(StackItem item)
{
    return (item.isNum == 0 && item.numPart == 0.0 && item.operatorPart == '\0');
}

//Convert function to char that represents a monocular operator. The mapping can be found in helper.txt.
char function_wrapper(char* function_identifier)
{
    if(!strcmp(function_identifier,"abs"))
        return (char)-1;
    else if(!strcmp(function_identifier,"exp"))
        return (char)-2;
    else if(!strcmp(function_identifier,"ln"))
        return (char)-3;
    else if(!strcmp(function_identifier,"lg"))
        return (char)4;
    else if(!strcmp(function_identifier,"sqrt"))
        return (char)-5;
    else if(!strcmp(function_identifier,"cbrt"))
        return (char)-6;
    else if(!strcmp(function_identifier,"sin"))
        return (char)-7;
    else if(!strcmp(function_identifier,"cos"))
        return (char)-8;
    else if(!strcmp(function_identifier,"tan"))
        return (char)-9;
    else if(!strcmp(function_identifier,"asin"))
        return (char)-10;
    else if(!strcmp(function_identifier,"acos"))
        return (char)-11;
    else if(!strcmp(function_identifier,"atan"))
        return (char)-12;
    else if(!strcmp(function_identifier,"sinh"))
        return (char)-13;
    else if(!strcmp(function_identifier,"cosh"))
        return (char)-14;
    else if(!strcmp(function_identifier,"tanh"))
        return (char)-15;
    else if(!strcmp(function_identifier,"asinh"))
        return (char)-16;
    else if(!strcmp(function_identifier,"acosh"))
        return (char)-17;
    else if(!strcmp(function_identifier,"atanh"))
        return (char)-18;
    else if(!strcmp(function_identifier,"erf"))
        return (char)-19;
    else if(!strcmp(function_identifier,"erfc"))
        return (char)-20;
    else if(!strcmp(function_identifier,"tgamma"))
        return (char)-21;
    else if(!strcmp(function_identifier,"lgamma"))
        return (char)-22;
    else if(!strcmp(function_identifier,"ceil"))
        return (char)-23;
    else if(!strcmp(function_identifier,"floor"))
        return (char)-24;
    else if(!strcmp(function_identifier,"trunc"))
        return (char)-25;
    else if(!strcmp(function_identifier,"round"))
        return (char)-26;
    else
        return (char)0;
}

//convert the special chars back to the function to calculate
double function_unwrapper(char wrapped_identifier,double function_operand)
{
    switch (wrapped_identifier)
    {
    case (char)-1:
        return fabs(function_operand);
    case (char)-2:
        return exp(function_operand);
    case (char)-3:
        return log(function_operand);
    case (char)-4:
        return log10(function_operand);
    case (char)-5:
        return sqrt(function_operand);
    case (char)-6:
        return cbrt(function_operand);
    case (char)-7:
        return sin(function_operand);
    case (char)-8:
        return cos(function_operand);
    case (char)-9:
        return tan(function_operand);
    case (char)-10:
        return asin(function_operand);
    case (char)-11:
        return acos(function_operand);
    case (char)-12:
        return atan(function_operand);
    case (char)-13:
        return sinh(function_operand);
    case (char)-14:
        return cosh(function_operand);
    case (char)-15:
        return tanh(function_operand);
    case (char)-16:
        return asinh(function_operand);
    case (char)-17:
        return acosh(function_operand);
    case (char)-18:
        return atanh(function_operand);
    case (char)-19:
        return erf(function_operand);
    case (char)-20:
        return erfc(function_operand);
    case (char)-21:
        return tgamma(function_operand);
    case (char)-22:
        return lgamma(function_operand);
    case (char)-23:
        return ceil(function_operand);
    case (char)-24:
        return floor(function_operand);
    case (char)-25:
        return trunc(function_operand);
    case (char)-26:
        return round(function_operand);
    case (char)-27:
        return function_operand;
    case (char)-28:
        return -function_operand;
    default:
        return function_operand;
    }
}

char* str_of_wrapped_function(char wrapped_identifier) 
{ 
    switch (wrapped_identifier) 
    { 
    case (char)-1: 
        return("abs"); 
    case (char)-2: 
        return("exp"); 
    case (char)-3: 
        return("ln"); 
    case (char)-4: 
        return("lg"); 
    case (char)-5: 
        return("sqrt"); 
    case (char)-6: 
        return("cbrt"); 
    case (char)-7: 
        return("sin"); 
    case (char)-8: 
        return("cos"); 
    case (char)-9: 
        return("tan"); 
    case (char)-10: 
        return("asin"); 
    case (char)-11: 
        return("acos"); 
    case (char)-12: 
        return("atan"); 
    case (char)-13: 
        return("sinh"); 
    case (char)-14: 
        return("cosh"); 
    case (char)-15: 
        return("tanh"); 
    case (char)-16: 
        return("asinh"); 
    case (char)-17: 
        return("acosh"); 
    case (char)-18: 
        return("atanh"); 
    case (char)-19: 
        return("erf"); 
    case (char)-20: 
        return("erfc"); 
    case (char)-21: 
        return("tgamma"); 
    case (char)-22: 
        return("lgamma"); 
    case (char)-23: 
        return("ceil"); 
    case (char)-24: 
        return("floor"); 
    case (char)-25: 
        return("trunc"); 
    case (char)-26: 
        return("round"); 
    default: 
        return NULL; 
    } 
}

double expr_parse(char *expr_input,int expr_length,double x)
{
    //make the two stacks used to convert expression
    char operator_stack[expr_length];
    memset(operator_stack,0,expr_length);
    StackItem generic_stack[expr_length];
    memset(generic_stack,0,expr_length*sizeof(StackItem));
    int operator_stack_top = -1;
    int generic_stack_top = -1;

    //temp / loop variables
    StackItem temp_item;
    int i,j,prev_i;
    char prev_char = '\0',next_char = '\0';

    //function-related variables
    char function_identifier[7] = {'\0'};
    int function_i = 0;
    int function_brackets = 0;
    char number_str[128] = {'\0'};
    char function_stack[expr_length];
    int function_stack_top = -1;
    memset(function_stack,0,expr_length);

    //sign(+/-)related variables
    char sign_operator = '\0';

    //check the number of brackets
    for(i = 0;i < expr_length; i++)
    {
        if(*(expr_input+i) == '(')
            function_brackets++;
        else if(*(expr_input+i) == ')')
            function_brackets--;
    }
    if(function_brackets)
    {
        errno = ESYNTAX_BASE+'(';
        return nan("0");
    }
    if(i)
    {
        i--;
        if(*(expr_input+i) == '(')
        {
            errno = ESYNTAX_BASE+'(';
            return nan("0");
        }
    }
    //Too long operand is not supported.
    for(i = 0;i < expr_length; i++)
    {
        CHECK_STACKS();
        if(i) 
            prev_char = *(expr_input+i-1);
        if(expr_length-1-i)
            next_char = *(expr_input+i+1);
        // Warning: The following sentence use 'case low ... high:',which is a C Extension.
        switch (*(expr_input+i))
        {
        //left bracket
        case '(':
            operator_stack_top++;
            operator_stack[operator_stack_top] = *(expr_input+i);
            //no identifier
            if(function_identifier[0] == '\0')
            {
                function_stack_top++;
                function_stack[function_stack_top] = '\0';
            }
            //has identifier
            else
            {
                function_stack_top++;
                function_stack[function_stack_top] = function_wrapper(function_identifier);
                memset(function_identifier,0,7);
            }
            break;

        //numbers
        case '0' ... '9':
            number_str[0] = *(expr_input+i);
            prev_i = i;
            for(;i < prev_i + 127;i++)
            {
                switch (*(expr_input+i))
                {
                case '0' ... '9':   case '.':   case 'E'    :case 'e':
                    number_str[i-prev_i] = *(expr_input+i);
                    break;
                case '+':   case '-':
                    if(i>0 && *(expr_input+i-1) == 'e' || *(expr_input+i-1) == 'E')
                    {
                            number_str[i-prev_i] = *(expr_input+i);
                    }
                    else
                    {
                        i--;
                        goto end_of_loop1;
                    }
                    break;
                case 'a' ... 'd': case 'f' ... 'z':
                    if(operator_stack_top == -1)
                    {
                        operator_stack_top++;
                        operator_stack[operator_stack_top] = '*';
                    }
                    else if(operator_precedence(operator_stack[operator_stack_top]) < operator_precedence('*'))
                    {
                        operator_stack_top++;
                        operator_stack[operator_stack_top] = '*';
                    }
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
            memset(number_str,0,sizeof(number_str));
            if(sign_operator)
            {
                CHECK_STACKS();
                temp_item.isNum = 0;
                temp_item.operatorPart = (sign_operator == '+') ? -27 : -28;
                generic_stack_top++;
                generic_stack[generic_stack_top] = temp_item;
                sign_operator = 0;
            }
            break;

        //operators
        case '+':   case '-':
            if(prev_char == '\(' || is_operator(prev_char) || !i)
            {
                if(next_char == '\(')
                {
                    function_stack_top++;
                    function_stack[function_stack_top] = (*(expr_input+i) == '+') ? -27 : -28;                    
                }
                else
                    sign_operator = *(expr_input+i);
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
                    CHECK_STACKS();
                    temp_item.isNum = 0;
                    temp_item.operatorPart = operator_stack[operator_stack_top];
                    operator_stack[operator_stack_top] = '\0';
                    operator_stack_top--;
                    generic_stack_top++;
                    generic_stack[generic_stack_top] = temp_item;
                } while ( operator_stack_top != -1 && operator_precedence(operator_stack[operator_stack_top]) >= operator_precedence(*(expr_input+i)));
                operator_stack_top++;
                operator_stack[operator_stack_top] = *(expr_input+i);
            }
        break;
        //functions
        case 'a' ... 'z':
        //x is the variable
            if(*(expr_input+i) == 'x')
            {
                temp_item.isNum = 1;
                temp_item.numPart = x;
                generic_stack_top++;
                generic_stack[generic_stack_top] = temp_item;
                break;
            }
            //read function identifier
            do
            {
                function_identifier[function_i] = *(expr_input+i);
                function_i++;
                i++;
            } while (!(*(expr_input+i) == '\(' || *(expr_input+i) == '\0')); 
            //no matched bracket 
            if(*(expr_input+i) == '\0') 
            { 
                errno = 1024+function_wrapper(function_identifier); 
                return nan("0");                 
            }
            function_i=0;
            i--;
            break;

        case ')':
            while (operator_stack[operator_stack_top] != '(')
            {
                CHECK_STACKS();
                temp_item.isNum = 0;
                temp_item.operatorPart = operator_stack[operator_stack_top];
                operator_stack[operator_stack_top] = '\0';
                operator_stack_top--;
                generic_stack_top++;
                generic_stack[generic_stack_top] = temp_item;
            }
            operator_stack[operator_stack_top] = '\0';
            operator_stack_top--;
            temp_item.isNum = 0;
            temp_item.operatorPart = function_stack[function_stack_top];
            generic_stack_top++;
            generic_stack[generic_stack_top] = temp_item;
            function_stack[function_stack_top] = '\0';
            function_stack_top--;
            break;
        case '\0':
            goto end_of_loop2;
        default:
            errno = ESYNTAX_BASE+*(expr_input+i);
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
        StackItem calculation_stack[expr_length];
        memset(calculation_stack,0,expr_length*sizeof(StackItem));
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
                        if(calculation_stack_top && calculation_stack[calculation_stack_top-1].isNum && calculation_stack[calculation_stack_top].isNum)
                        {
                            calculation_stack[calculation_stack_top-1].numPart += calculation_stack[calculation_stack_top].numPart;
                            calculation_stack[calculation_stack_top] = empty_item;
                            calculation_stack_top--;                            
                        } 
                        else
                        {
                            errno = (calculation_stack[calculation_stack_top].isNum) ? (ESYNTAX_BASE+calculation_stack[calculation_stack_top-1].operatorPart):(ESYNTAX_BASE+calculation_stack[calculation_stack_top].operatorPart);
                            return nan("0");
                        }
                        break;
                    case '-':
                        if(calculation_stack_top && calculation_stack[calculation_stack_top-1].isNum && calculation_stack[calculation_stack_top].isNum)
                        {
                            calculation_stack[calculation_stack_top-1].numPart -= calculation_stack[calculation_stack_top].numPart;
                            calculation_stack[calculation_stack_top] = empty_item;
                            calculation_stack_top--;                            
                        } 
                        else
                        {
                            errno = (calculation_stack[calculation_stack_top].isNum) ? (ESYNTAX_BASE+calculation_stack[calculation_stack_top-1].operatorPart):(ESYNTAX_BASE+calculation_stack[calculation_stack_top].operatorPart);
                            return nan("0");
                        }
                        break;
                    case '*':
                        if(calculation_stack_top && calculation_stack[calculation_stack_top-1].isNum && calculation_stack[calculation_stack_top].isNum)
                        {
                            calculation_stack[calculation_stack_top-1].numPart *= calculation_stack[calculation_stack_top].numPart;
                            calculation_stack[calculation_stack_top] = empty_item;
                            calculation_stack_top--;                            
                        } 
                        else
                        {
                            errno = (calculation_stack[calculation_stack_top].isNum) ? (ESYNTAX_BASE+calculation_stack[calculation_stack_top-1].operatorPart):(ESYNTAX_BASE+calculation_stack[calculation_stack_top].operatorPart);
                            return nan("0");
                        }
                        break;
                    case '/':
                        if(calculation_stack_top && calculation_stack[calculation_stack_top-1].isNum && calculation_stack[calculation_stack_top].isNum)
                        {
                            calculation_stack[calculation_stack_top-1].numPart /= calculation_stack[calculation_stack_top].numPart;
                            calculation_stack[calculation_stack_top] = empty_item;
                            calculation_stack_top--;                            
                        } 
                        else
                        {
                            errno = (calculation_stack[calculation_stack_top].isNum) ? (ESYNTAX_BASE+calculation_stack[calculation_stack_top-1].operatorPart):(ESYNTAX_BASE+calculation_stack[calculation_stack_top].operatorPart);
                            return nan("0");
                        }
                        break;
                    case '^':
                        if(calculation_stack_top && calculation_stack[calculation_stack_top-1].isNum && calculation_stack[calculation_stack_top].isNum)
                        {
                            calculation_stack[calculation_stack_top-1].numPart = pow(calculation_stack[calculation_stack_top-1].numPart,calculation_stack[calculation_stack_top].numPart);
                            calculation_stack[calculation_stack_top] = empty_item;
                            calculation_stack_top--;                            
                        } 
                        else
                        {
                            errno = (calculation_stack[calculation_stack_top].isNum) ? (ESYNTAX_BASE+calculation_stack[calculation_stack_top-1].operatorPart):(ESYNTAX_BASE+calculation_stack[calculation_stack_top].operatorPart);
                            return nan("0");
                        }
                        break;        
                    default:
                        break;
                    }
                }
                else if(generic_stack[i].operatorPart == 0)
                {
                    ;
                }
                else
                {
                    calculation_stack[calculation_stack_top].numPart = function_unwrapper(generic_stack[i].operatorPart,calculation_stack[calculation_stack_top].numPart);
                }
            }
        }
        return calculation_stack[0].numPart;
}
