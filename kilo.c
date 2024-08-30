/** includes **/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/** defines */

#define CTRL(key) ((key) & 0x1f)

/** data **/

struct termios ORIGINAL_TERMINAL_ATTRIBUTES;

/** screen */

void clearScreen() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}

/** utils **/

void die(const char *msg) {
  clearScreen();
  perror(msg);
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

char input_readKey() {
  int countRead = 0;
  char input = '\0';
  do {
    countRead = read(STDIN_FILENO, &input, 1);
    if (countRead == -1 && errno != EAGAIN) {
      die("read");
    }
  } while (countRead != 1);
  return input;
}

/** input */

void editor_processKeyPress() {
  char c = input_readKey();
  switch (c) {
  case CTRL('q'):
    exit(0);
    break;

  default:
    break;
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
    clearScreen();
    editor_processKeyPress();
  }
  return 0;
}
