#include <stdio.h>
#include "exprparser.h"
#include "keyboard.h"
#ifdef _WIN32
#include <Windows.h>
#define sleep(sec)   Sleep(sec * 1000)
#endif
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
            switch(c)
            {
                case '0' ... '9':
                case 'a' ... 'z':
                case '+': case '-': case '*': case '/': case '^':
                case '(': case ')':
                    input_expression[head] = c;
                    head++;
                    break;
                default:
                    printf("\007");
                    break;
            }

            printf("\033[2J\033[H");
            printf("Input:%s\n",input_expression);
            printf("Output:%.9g\n",expr_parse(input_expression,128,x));
        }
        sleep(0.005);
    }
}