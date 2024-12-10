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
    int cursor_pos = -1;
    int top = -1;
    int i = 0;
    char c = 0;
    int flag_end_loop = 1;
    begin_read_key();
    while(flag_end_loop)
    {
        sleep(0.005);
        c = key_pressed_fast();
        if(c)
        {
            putchar(c);
            switch(c)
            {
                //common numbers & symbols
                case '0' ... '9':
                case 'a' ... 'z':
                case '+': case '-': case '*': case '/': case '^':
                case '(': case ')':
                    if(top >= 0 && top < 128)
                    {
                        if(cursor_pos != top)
                        {
                            for ( i = top; i >= cursor_pos+1; i--)
                            {
                                input_expression[i+1] = input_expression[i];
                            }
                        }
                        top++;
                        cursor_pos++; 
                        input_expression[cursor_pos] = c;
                  
                    }
                    else if(top == -1)
                    {
                        input_expression[0] = c;
                        top++;
                        cursor_pos++;                        
                    }
                    break;
                case 27:
                    if(!kbhit())
                        flag_end_loop = 0;
                #ifdef _WIN32
                    break;
                case (char)224:
                    switch (key_pressed_fast())
                    {
                    case 75:
                        cursor_pos--;
                        break;
                    case 77:
                        cursor_pos++;
                        break;
                    default:
                        break;
                    }
                    break;
                #elif defined __linux__
                //ESC and control sequence
                    else
                    {
                        if(key_pressed_fast() == '[')
                        {
                            switch (key_pressed_fast())
                            {
                            case 'D':
                                if(cursor_pos >= 0)
                                    cursor_pos--;
                                break;
                            case 'C':
                                if(cursor_pos < top)
                                    cursor_pos++;
                                break;
                            default:
                                break;
                        };                            
                        }

                    }
                    break;
                #endif
                //backspace
                case 8:    case 127:    case '\\':
                if(cursor_pos == -1)
                    break;
                if(top >= 0)
                {
                    for(i = cursor_pos+1;i <= top; i++)
                        input_expression[i-1] = input_expression[i];
                    input_expression[top] = '\0';
                    if(cursor_pos >= 0)
                        cursor_pos--;
                    top--;
                }
                    break;
                default:
                    printf("\007");
                    break;
            }
            printf("\033[2J\033[H");
            printf("Input:");
            for(i = 0; i <= top; i++)
            {
                if(i == cursor_pos+1)
                {
                    printf("\033[47m");
                    putchar(input_expression[i]);
                    printf("\033[0m");
                }
                else
                {
                    putchar(input_expression[i]);
                }
            }
            if(cursor_pos >= top)
                printf("\033[47m \033[0m");
            putchar('\n');
            printf("Output:%.9g\n",expr_parse(input_expression,128,x));
            if(errno > 1024 && errno)
            {
                if(errno-1024 == '(')
                    puts("Error:Unmatched or unclosed brackets");
                else
                    printf("Error:Unexpected symbol \"%c\"\n",errno-1024);
            }
            else if(errno == 1024)
            {
                printf("Error:Undefined syntax error\n");
            }
            else if(errno != 0)
            {
                perror("Error");
            }
        errno = 0;
    }
    }
    end_read_key();
}