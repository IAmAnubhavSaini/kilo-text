#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios ORIGINAL_TERMINAL_ATTRIBUTES;

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &ORIGINAL_TERMINAL_ATTRIBUTES);
}

void cfmakeraw(struct termios *termios_p)
{
    termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    termios_p->c_oflag &= ~OPOST;
    termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    termios_p->c_cflag &= ~(CSIZE | PARENB);
    termios_p->c_cflag |= CS8;
}

void enableRawMode()
{
    atexit(disableRawMode);
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    cfmakeraw(&raw);
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
        if (iscntrl(c))
        {
            printf("^%d\n", c);
        }
        else
        {
            printf("%d ('%c')\n", c, c);
        }
    }
    return 0;
}
