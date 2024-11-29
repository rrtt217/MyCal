#pragma once
#include <stdio.h>

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
//Return the keys being pressed.Return 0 if failed, and some keys may be interpreted as multiple chars.
char key_pressed(void)
{
     //disable buffer
     struct termios t;
     tcgetattr(STDIN_FILENO, &t);
     t.c_lflag &= ~(ICANON | ECHO);
     tcsetattr(STDIN_FILENO, TCSANOW, &t);

     //set nonblock io
     int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
     fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

     //read and return
     char c;
     int return_status = read(STDIN_FILENO, &c, 1);
     if(return_status > 0)
     {
          //re-enable buffer
          tcgetattr(STDIN_FILENO, &t);
          t.c_lflag |= (ICANON | ECHO);
          tcsetattr(STDIN_FILENO, TCSANOW, &t);
          //reset blocked io
          flags &= ~O_NONBLOCK;
          fcntl(STDIN_FILENO, F_SETFL, flags);
          return c;          
     }
     else if(return_status == -1)
     {
     //no key was pressed.
          //re-enable buffer
          tcgetattr(STDIN_FILENO, &t);
          t.c_lflag |= (ICANON | ECHO);
          tcsetattr(STDIN_FILENO, TCSANOW, &t);
          //reset blocked io
          flags &= ~O_NONBLOCK;
          fcntl(STDIN_FILENO, F_SETFL, flags);
          return 0;          
     }

}
void begin_read_key(void)
{
     //disable buffer
     struct termios t;
     tcgetattr(STDIN_FILENO, &t);
     t.c_lflag &= ~(ICANON | ECHO);
     tcsetattr(STDIN_FILENO, TCSANOW, &t);

     //set nonblock io
     int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
     fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

}
void end_read_key(void)
{
     struct termios t;
     tcgetattr(STDIN_FILENO, &t);
     int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
     //re-enable buffer
     tcgetattr(STDIN_FILENO, &t);
     t.c_lflag |= (ICANON | ECHO);
     tcsetattr(STDIN_FILENO, TCSANOW, &t);
     //reset blocked io
     flags &= ~O_NONBLOCK;
     fcntl(STDIN_FILENO, F_SETFL, flags);
}
char key_pressed_fast(void)
{
          //read and return
     char c;
     int return_status = read(STDIN_FILENO, &c, 1);
     if(return_status > 0)
     {
          return c;          
     }
     else if(return_status == -1)
     {
     //no key was pressed.
          return 0;          
     }
}
#elif defined _WIN32
#include <conio.h>
//Return the keys being pressed.Return 0 if failed, and some keys may be interpreted as multiple chars.
char key_pressed(void)
{
     if(kbhit())
          return getch();
     else
          return 0;
}
//Return the keys being pressed.Return 0 if failed, and some keys may be interpreted as multiple chars.
char key_pressed_fast(void)
{
     return key_pressed();
}
void begin_read_key(void)
{}
void end_read_key(void)
{}
#endif