#pragma once
#include <stdio.h>

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
//Return the keys being pressed.Return 0 if failed, and some keys may be interpreted as multiple chars.
char key_pressed(void)
{
     //set nonblock io
     int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
     fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

     //read and return
     char c;
     int return_status = read(STDIN_FILENO, &c, 1);
     if(return_status > 0)
          return c;
     else if(return_status == -1)
     //no key was pressed.
          return 0;
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
#endif