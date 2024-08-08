#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

struct termios ORIGINAL_TERMINAL_ATTRIBUTES;

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &ORIGINAL_TERMINAL_ATTRIBUTES);
}

void enableRawMode()
{
    atexit(disableRawMode);
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    if (raw.c_lflag & ECHO)
    {
        raw.c_lflag &= ~ECHO;
    }
    if (raw.c_lflag & ICANON)
    {
        raw.c_lflag &= ~ICANON;
    }
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void initialise_first()
{
    tcgetattr(STDIN_FILENO, &ORIGINAL_TERMINAL_ATTRIBUTES);
}

int main()
{
    initialise_first();
    enableRawMode();

    printf("Hello from kilo!\n");
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (c == 'q')
        {
            break;
        }
    }
    return 0;
}
