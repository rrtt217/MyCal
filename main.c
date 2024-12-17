#include <stdio.h>
#include "exprparser.h"
#include "keyboard.h"
#include "str_list.h"
#ifdef _WIN32
#include <Windows.h>
#define sleep(sec)   Sleep(sec * 1000)
#endif
int main()
{
    char empty_expression[128] = {"\0"};

    int top = -1;
    int x = 1;
    int cursor_pos = -1;

    int i = 0;
    char c = 0;
    int flag_end_loop = 1;
    node *list_head = create_list();
    list_head = append_item(list_head,empty_expression,128);
    node* current = list_head->next;
    int n = 1;
    
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
                                current->str_data[i+1] = current->str_data[i];
                            }
                        }
                        top++;
                        cursor_pos++; 
                        current->str_data[cursor_pos] = c;
                    }
                    else if(top == -1)
                    {
                        current->str_data[0] = c;
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
                            list_head = auto_delete(list_head,current);
                            switch (key_pressed_fast())
                            {
                            case 'A':
                                if(current->prev && current->prev->prev)
                                    current = current->prev;
                                break;
                            case 'B':
                                if(current->next)
                                    current = current->next;
                                else
                                {
                                    list_head = append_item(list_head,empty_expression,128);
                                    current = current->next;
                                }
                                break;
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
                        current->str_data[i-1] = current->str_data[i];
                    current->str_data[top] = '\0';
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
            if(list_head->next != current)
                n = print_list_segment(list_head->next,current->prev,0,x);
            printf("Input[%d]:",n+1);
            for(i = 0; i <= top; i++)
            {
                if(i == cursor_pos+1)
                {
                    printf("\033[47m");
                    putchar(current->str_data[i]);
                    printf("\033[0m");
                }
                else
                {
                    putchar(current->str_data[i]);
                }
            }
            if(cursor_pos >= top)
                printf("\033[47m \033[0m");
            putchar('\n');
            printf("Output:%.9g\n",expr_parse(current->str_data,128,x));
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
            else if(errno > 1024-128)
            {
                if(str_of_wrapped_function(errno-1024))
                    printf("Error:Function %s\() has no brackets matched\n",str_of_wrapped_function(errno-1024));
            }
            else if(errno != 0)
            {
                perror("Error");
            }
        errno = 0;
        print_list_segment(current->next,NULL,n+1,x);
        n = 0;
    }
    }
    end_read_key();
}