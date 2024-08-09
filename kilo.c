#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void readShenanighans(struct termios *t)
{
    t->c_cc[VMIN] = 0;
    t->c_cc[VTIME] = 1;
}

void enableRawMode()
{
    atexit(disableRawMode);
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    cfmakeraw(&raw);
    readShenanighans(&raw);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void initialise_first()
{
    tcgetattr(STDIN_FILENO, &ORIGINAL_TERMINAL_ATTRIBUTES);
}

void clearBuffer(char *buffer, size_t size)
{
    memset(buffer, 0, size);
}

int main()
{
    initialise_first();
    enableRawMode();

    char buffer[1000];
    int len = snprintf(buffer, sizeof(buffer), "Hello from kilo!\r\n");
    write(STDOUT_FILENO, buffer, len);
    clearBuffer(buffer, sizeof(buffer));

    while (1)
    {
        char c = '\0';
        read(STDIN_FILENO, &c, 1);
        if (c == 'q')
        {
            break;
        }
        if (iscntrl(c))
        {
            int len = snprintf(buffer, sizeof(buffer), "^%d\r\n", c);
            write(STDOUT_FILENO, buffer, len);
            clearBuffer(buffer, sizeof(buffer));
        }
        else
        {
            int len = snprintf(buffer, sizeof(buffer), "%d ('%c')\r\n", c, c);
            write(STDOUT_FILENO, buffer, len);
            clearBuffer(buffer, sizeof(buffer));
        }
    }
    return 0;
}
