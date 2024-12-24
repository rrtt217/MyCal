#pragma once
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ESYNTAX_BASE 1024
//The char that causes ESYNTAX will be added with ESYNTAX_BASE.In some special cases it works differently.


#define CHECK_STACKS()         if(operator_stack_top < -1 || generic_stack_top < -1){errno = ESYNTAX_BASE+'(';return nan("0");}

typedef struct {
 char operatorPart;
 char isNum;
 double numPart;
}StackItem;

int is_operator(char c);

int operator_precedence(char c);

int is_num(char c);

int is_lowercase_alpha(char c);

int is_empty(StackItem item);

//Convert function to char that represents a monocular operator. The mapping can be found in helper.txt.
char function_wrapper(char* function_identifier);


//convert the special chars back to the function to calculate
double function_unwrapper(char wrapped_identifier,double function_operand);

char* str_of_wrapped_function(char wrapped_identifier);

double expr_parse(char *expr_input,int expr_length,double x);
