#include <stdio.h>
#include <unistd.h>

int main()
{
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
