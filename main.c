#include <stdio.h>
#include "exprparser.h"
#include "keyboard.h"
int main()
{
    char input_expression[128] = {"\0"};
    int x = 1;
    int head = 0;
    char c = 0;
    while(1)
    {
        c = key_pressed();
        if(c)
        {
            input_expression[head] = c;
            head++;
            system("clear");
            printf("Input:%s\n\033[J",input_expression);
            printf("Output:%F",expr_parse(input_expression,128,x));
        }
        sleep(0.005);
    }
}