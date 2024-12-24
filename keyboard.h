#pragma once
#include <stdio.h>

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
//Return the key being pressed immediately.Return 0 if failed, and some keys may be interpreted as multiple chars.
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
//On linux it should be used before key_pressed_fast().On Windows it is just a dummy function.
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
//On linux it should be used after key_pressed_fast().On Windows it is just a dummy function.
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
//A faster function to read keys by not change terminal setting every time it is called.Use it with begin_read_key() and end_read_key().
char key_pressed_fast(void)
{
     //read and return
     char c;
     int errno_before = errno;
     int return_status = read(STDIN_FILENO, &c, 1);
     if(return_status > 0)
     {
          return c;          
     }
     else if(return_status == -1)
     {
     //no key was pressed.
          errno = errno_before;
          return 0;          
     }
}
#include <sys/select.h>
#include <sys/time.h>
int kbhit()
{
     struct timeval tv;
     fd_set readfds;
     
     //wait for 1ms
     tv.tv_sec = 0;
     tv.tv_usec = 0;    

     FD_ZERO(&readfds);
     FD_SET(STDIN_FILENO, &readfds); 
     return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
}
#elif defined _WIN32
#include <conio.h>
//Return the key being pressed.Return 0 if failed, and some keys may be interpreted as multiple chars.
char key_pressed(void)
{
     if(kbhit())
          return getch();
     else
          return 0;
}
//Return the key being pressed.Return 0 if failed, and some keys may be interpreted as multiple chars.
char key_pressed_fast(void)
{
     return key_pressed();
}
//On linux it should be used before key_pressed_fast().On Windows it is just a dummy function.
void begin_read_key(void)
{}
//On linux it should be used after key_pressed_fast().On Windows it is just a dummy function.
void end_read_key(void)
{}
#endif