#include <stdio.h>
#include "ini.h"
#include "exprparser.h"
#include "keyboard.h"
#ifdef _WIN32
#include <Windows.h>
#define sleep(sec) Sleep(sec * 1000)
#endif
#include <time.h>
#include <omp.h>

typedef struct 
{
    int max_expr_size;
    int x_half_width;
    int y_half_width;
    double x_extra_scale;
    int search_times;
}configuration;

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)user;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if(MATCH("General","max_expr_size"))
        pconfig->max_expr_size = atoi(value);
    else if(MATCH("Graphics","x_half_width"))
        pconfig->x_half_width = atoi(value);
    else if(MATCH("Graphics","y_half_width"))
        pconfig->y_half_width = atoi(value);
    else if(MATCH("Graphics","x_extra_scale"))
        pconfig->x_extra_scale = atof(value);
    else if(MATCH("Graphics","search_times"))
        pconfig->search_times = atoi(value);
    else
        return 0;
    return 1;
}

void simple_calculator(configuration config){
    char input_expression[config.max_expr_size];
    memset(input_expression,0,config.max_expr_size);
    int x = 1;
    int cursor_pos = -1;
    int top = -1;
    int i = 0;
    char c = 0;
    int flag_end_loop = 1;
    begin_read_key();
    while (flag_end_loop)
    {
        sleep(0.005);
        c = key_pressed_fast();
        if (c)
        {
            putchar(c);
            switch (c)
            {
            // common numbers & symbols
            case '0' ... '9':
            case 'a' ... 'z':
            case '+':
            case '-':
            case '*':
            case '/':
            case '^':
            case '(':
            case ')':
                if (top >= 0 && top < config.max_expr_size)
                {
                    if (cursor_pos != top)
                    {
                        for (i = top; i >= cursor_pos + 1; i--)
                        {
                            input_expression[i + 1] = input_expression[i];
                        }
                    }
                    top++;
                    cursor_pos++;
                    input_expression[cursor_pos] = c;
                }
                else if (top == -1)
                {
                    input_expression[0] = c;
                    top++;
                    cursor_pos++;
                }
                break;
            case 27:
                if (!kbhit())
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
                // ESC and control sequence
                else
                {
                    if (key_pressed_fast() == '[')
                    {
                        switch (key_pressed_fast())
                        {
                        case 'D':
                            if (cursor_pos >= 0)
                                cursor_pos--;
                            break;
                        case 'C':
                            if (cursor_pos < top)
                                cursor_pos++;
                            break;
                        default:
                            break;
                        };
                    }
                }
                break;
#endif
            // backspace
            case 8:
            case 127:
            case '\\':
                if (cursor_pos == -1)
                    break;
                if (top >= 0)
                {
                    for (i = cursor_pos + 1; i <= top; i++)
                        input_expression[i - 1] = input_expression[i];
                    input_expression[top] = '\0';
                    if (cursor_pos >= 0)
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
            for (i = 0; i <= top; i++)
            {
                if (i == cursor_pos + 1)
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
            if (cursor_pos >= top)
                printf("\033[47m \033[0m");
            putchar('\n');
            printf("Output:%.9g\n", expr_parse(input_expression, config.max_expr_size, x));
            if (errno > 1024 && errno)
            {
                if (errno - 1024 == '(')
                    puts("Error:Unmatched or unclosed brackets");
                else
                    printf("Error:Unexpected symbol \"%c\"\n", errno - 1024);
            }
            else if (errno == 1024)
            {
                printf("Error:Undefined syntax error\n");
            }
            else if (errno > 1024 - config.max_expr_size)
            {
                if (str_of_wrapped_function(errno - 1024))
                    printf("Error:Function %s\() has no brackets matched\n", str_of_wrapped_function(errno - 1024));
            }
            else if (errno != 0)
            {
                perror("Error");
            }
            errno = 0;
        }
    }
    end_read_key();
}

void graphical_calculator(configuration config)
{
    char expression[config.max_expr_size];
    char line_buf[12*(2*config.x_half_width+1)];
    memset(expression,0,config.max_expr_size);
    memset(line_buf,0,12*(2*config.x_half_width+1));
    int line_buf_top = -1;
    printf("Input expression to draw:");
    scanf("%127s",expression);
    double center_x = 0 , center_y = 0 , scale = 0.1;
    int scry;
    //int scrx,scry,i;
    int x_axis_pos,y_axis_pos;
    char render_frame = 0 , flag_loop = 1;
    char c;
    int min_values[2*config.x_half_width+1] , max_values[2*config.x_half_width+1] , now_values[2*config.x_half_width+1];
    int min_value,max_value, now_value;
    clock_t frame_start , frame_cost;
    printf("\033[2J\033[H");
    while (flag_loop)
    {
        frame_start = clock();
        begin_read_key();
        c = key_pressed_fast();
        switch (c)
        {
        case 'w':
            center_y += scale * config.y_half_width / 20.0;
            render_frame = 1;
            break;
        case 's':
            center_y -= scale * config.y_half_width / 20.0;
            render_frame = 1;
            break;
        case 'a':
            center_x -= scale * config.x_half_width / 20.0 * config.x_extra_scale;
            render_frame = 1;
            break;
        case 'd':
            center_x += scale * config.x_half_width / 20.0 * config.x_extra_scale;
            render_frame = 1;
            break;
        case 'z':
            scale *= 0.9;
            render_frame = 1;
            break;
        case 'm':
            scale *= 1.1;
            render_frame = 1;
            break;
        case 27:
            if (!kbhit())
                flag_loop = 0;
#ifdef _WIN32
            break;
        case (char)224:
            switch (key_pressed_fast())
            {
            case 72:
            center_y += scale * config.y_half_width / 20.0;
            render_frame = 1;
                break;
            case 80:
            center_y -= scale * config.y_half_width / 20.0;
            render_frame = 1;
            break;
            case 75:
            center_x -= scale * config.x_half_width / 20.0 * config.x_extra_scale;
            render_frame = 1;
                break;
            case 77:
            center_x += scale * config.x_half_width / 20.0 * config.x_extra_scale;
            render_frame = 1;
                break;
            default:
                break;
            }
            break;
#elif defined __linux__
            // ESC and control sequence
            else
            {
                if (key_pressed_fast() == '[')
                {
                    switch (key_pressed_fast())
                    {
                    case 'A':
                        center_y += scale * config.y_half_width / 20.0;
                        render_frame = 1;
                        break;
                    case 'B':
                        center_y -= scale * config.y_half_width / 20.0;
                        render_frame = 1;
                        break;
                    case 'C':
                        center_x += scale * config.x_half_width / 20.0 * config.x_extra_scale;
                        render_frame = 1;
                        break;
                    case 'D':
                        center_x -= scale * config.x_half_width / 20.0 * config.x_extra_scale;
                        render_frame = 1;
                        break;
                    default:
                        break;
                    };
                }
            }
            break;
#endif
        default:
            break;
        }
        end_read_key();
        if (render_frame)
        {
            render_frame = 0;
            printf("\033[H");
            printf("Expression:%s   Scale:%.2g    x:%.9g   y:%.9g\n",expression,scale,center_x,center_y);
            #pragma omp parallel for
            for(int scrx = -config.x_half_width;scrx <= config.x_half_width;scrx++)
            {
                for(int i = 0 ; i < config.search_times ; i++)
                {
                    now_values[scrx+config.x_half_width] = (int)((center_y - expr_parse(expression,128,((double)scrx / config.x_extra_scale - 0.5 + (double)i / config.search_times)*scale+center_x))/scale);
                    if(i == 0)
                        {
                            min_values[scrx+config.x_half_width] = now_values[scrx+config.x_half_width];
                            max_values[scrx+config.x_half_width] = now_values[scrx+config.x_half_width];
                        }
                    else if(now_values[scrx+config.x_half_width] < min_values[scrx+config.x_half_width])
                        min_values[scrx+config.x_half_width] = now_values[scrx+config.x_half_width];
                    else if(now_values[scrx+config.x_half_width] > max_values[scrx+config.x_half_width])
                        max_values[scrx+config.x_half_width] = now_values[scrx+config.x_half_width];
                }
            }
            x_axis_pos = (int)(0.5 - center_x * config.x_extra_scale / scale);
            y_axis_pos = (int)(0.5 + center_y / scale);
            int scrx;
            for(scry = -config.y_half_width;scry <= config.y_half_width;scry++)
            {
                for (scrx = -config.x_half_width; scrx <= config.x_half_width; scrx++)
                {
                    if (scry == y_axis_pos && scrx == x_axis_pos)
                    {
                        line_buf[line_buf_top+1] = '+';
                        line_buf_top++;
                    }
                    else if (scry == y_axis_pos)
                    {
                        line_buf[line_buf_top+1] = '=';
                        line_buf_top++;
                    }
                    else if (scrx == x_axis_pos)
                    {
                        line_buf[line_buf_top+1] = '|';
                        line_buf_top++;
                    }
                    else if (min_values[scrx + config.x_half_width] <= scry && scry <= max_values[scrx + config.x_half_width])
                    {
                        strcpy(line_buf+line_buf_top+1,"\033[47m \033[49m");
                        line_buf_top += 11;                        
                    }
                    else
                    {
                        line_buf[line_buf_top+1] = ' ';
                        line_buf_top++;
                    }
                }
                puts(line_buf);
                memset(line_buf,0,12*(2*config.x_half_width+1)*sizeof(char));
                line_buf_top = -1;
                //putchar('\n');
            }
            printf("\033[49m");
            printf("Time cost:%.9gs",((double)(clock()-frame_start))/CLOCKS_PER_SEC);
            memset(min_values,0,sizeof(int)*(2*config.x_half_width+1));
            memset(max_values,0,sizeof(int)*(2*config.x_half_width+1));
            memset(now_values,0,sizeof(int)*(2*config.x_half_width+1));
        }
    }
}
int main(){
    configuration config;
    //default config
    config.max_expr_size = 128;
    config.x_half_width = 168;
    config.y_half_width = 40;
    config.x_extra_scale = 1.8;
    config.search_times = 5;

    if(ini_parse("config.ini",handler,&config) < 0)
        printf("Failed to load config,use default");
    else
    {
        printf("Successful to load config");
    }

    char c1 = '*', c2 = ' ', c = '\0';
    char flag_loop = 1;
    while (flag_loop)
    {
        sleep(0.05);
        printf("\033[2J\033[H");
        printf("         Main Menu         \n");
        printf("%c1.simple calculator\n", c1);
        printf("%c2.\"graphical\" calculator\n", c2);
        begin_read_key();
        c = key_pressed_fast();
        switch (c)
        {
        case 'w':
            if (c1 == ' ')
            {
                c1 = '*';
                c2 = ' ';
            }
            break;
        case 's':
            if (c2 == ' ')
            {
                c2 = '*';
                c1 = ' ';
            }
            break;
        case 10:
        case 13:
            if (c1 == '*')
            {
                printf("\033[2J\033[H");
                end_read_key();
                simple_calculator(config);
            }

            else if (c2 == '*')
            {
                printf("\033[2J\033[H");
                end_read_key();
                graphical_calculator(config);
            }
            break;
        case 27:
            if (!kbhit())
                flag_loop = 0;
#ifdef _WIN32
            break;
        case (char)224:
            switch (key_pressed_fast())
            {
            case 72:
                if (c1 == ' ')
                {
                    c1 = '*';
                    c2 = ' ';
                }
                break;
            case 80:
                if (c2 == ' ')
                {
                    c2 = '*';
                    c1 = ' ';
                }
                break;
            default:
                break;
            }
            break;
#elif defined __linux__
            // ESC and control sequence
            else
            {
                if (key_pressed_fast() == '[')
                {
                    switch (key_pressed_fast())
                    {
                    case 'A':
                        if (c1 == ' ')
                        {
                            c1 = '*';
                            c2 = ' ';
                        }
                        break;
                    case 'B':
                        if (c2 == ' ')
                        {
                            c2 = '*';
                            c1 = ' ';
                        }
                        break;
                    default:
                        break;
                    };
                }
            }
            break;
#endif
        default:
            break;
        }
        end_read_key();
    }
}
