/** includes **/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/** defines */

/**
 * Why Use 0x1F?
 * Using 0x1F ensures that only the lower 5 bits of the key are kept,
 * which aligns with the way control characters are encoded in ASCII.
 * This macro is useful when you need to convert a regular character
 * to its corresponding control character.
 */
#define CTRL(key) ((key) & 0x1f)

/** data **/

struct termios ORIGINAL_TERMINAL_ATTRIBUTES;

/** utils **/

void die(const char *msg) {
  perror(msg);
  perror("logout :xd");
  exit(1);
}

void clearBuffer(char *buffer, size_t size) { memset(buffer, 0, size); }

/** terminal **/

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &ORIGINAL_TERMINAL_ATTRIBUTES) == -1) {
    die("tcsetattr ORIGINAL_TERMINAL_ATTRIBUTES");
  }
}

void cfmakeraw(struct termios *termios_p) {
  termios_p->c_iflag &=
      ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  termios_p->c_oflag &= ~OPOST;
  termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  termios_p->c_cflag &= ~(CSIZE | PARENB);
  termios_p->c_cflag |= CS8;
}

void readShenanighans(struct termios *t) {
  t->c_cc[VMIN] = 0;
  t->c_cc[VTIME] = 1;
}

void enableRawMode() {
  atexit(disableRawMode);
  struct termios raw;
  if (tcgetattr(STDIN_FILENO, &raw) == -1) {
    die("tcgetattr raw");
  }
  cfmakeraw(&raw);
  readShenanighans(&raw);
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    die("tcsetattr raw");
  }
}

/** init **/

void initialise_first() {
  if (tcgetattr(STDIN_FILENO, &ORIGINAL_TERMINAL_ATTRIBUTES) == -1) {
    die("tcgetattr ORIGINAL_TERMINAL_ATTRIBUTES");
  }
}

int main() {
  initialise_first();
  enableRawMode();

  char buffer[1000];
  int len = snprintf(buffer, sizeof(buffer), "Hello from kilo!\r\n");
  write(STDOUT_FILENO, buffer, len);
  clearBuffer(buffer, sizeof(buffer));

  while (1) {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) {
      die("read");
    }
    if (c == CTRL('q')) {
      break;
    }
    if (iscntrl(c)) {
      int len = snprintf(buffer, sizeof(buffer), "^%d\r\n", c);
      write(STDOUT_FILENO, buffer, len);
      clearBuffer(buffer, sizeof(buffer));
    } else {
      int len = snprintf(buffer, sizeof(buffer), "%d ('%c')\r\n", c, c);
      write(STDOUT_FILENO, buffer, len);
      clearBuffer(buffer, sizeof(buffer));
    }
  }
  return 0;
}
