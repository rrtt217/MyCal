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
    int top = -1;
    char c = 0;
    begin_read_key();
    while(1)
    {
        sleep(0.005);
        c = key_pressed_fast();
        if(c)
        {
            switch(c)
            {
                //common numbers & symbols
                case '0' ... '9':
                case 'a' ... 'z':
                case '+': case '-': case '*': case '/': case '^':
                case '(': case ')':
                    top++;
                    input_expression[top] = c;
                    break;
                
                //ESC and control sequence
                case 27:
                    break;
                //backspace
                case 8:    case 127:    case '\\':
                if(top >= 0)
                {
                    input_expression[top] = '\0';
                    top--;
                }

                    break;
                default:
                    printf("\007");
                    break;
            }
            printf("\033[2J\033[H");
            printf("Input:%s\n",input_expression);
            printf("Output:%.9g\n",expr_parse(input_expression,128,x));
            printf("Error:%d\n",errno);
            if(errno > 1024 && errno)
            {
                if(errno-1024 == '(')
                    puts("Error:Unmatched or unclosed brackets");
                else
                    printf("Error:Unexpected symbol \"%c\"\n",errno-1024);
            }
        else if(errno)
        {
            perror("Error");
        }
        errno = 0;
    }
    }
    end_read_key();
}