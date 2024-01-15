#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int serial_port;
char buffer[2];
char buffer_f[5];
ssize_t bytes_read;
void match_input();
int my_atoi(char *);
int main() {
  serial_port = open("/dev/ttyS0", O_RDWR); // Adjust the port as needed
  struct termios tty;

  if (serial_port < 0) {
    perror("Error opening serial port");
    return 1;
  }

  memset(&tty, 0, sizeof(tty));
  if (tcgetattr(serial_port, &tty) != 0) {
    perror("Error from tcgetattr");
    return 1;
  }

  cfsetospeed(&tty, B9600);
  cfsetispeed(&tty, B9600);

  tty.c_cflag &= ~PARENB; // No parity bit
  tty.c_cflag &= ~CSTOPB; // 1 stop bit
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8; // 8 data bits

  tty.c_cflag &= ~CRTSCTS;       // No hardware flow control
  tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem control lines

  tty.c_lflag &= ~ICANON; // Disable canonical mode
  tty.c_lflag &= ~ECHO;   // Disable echo
  tty.c_lflag &= ~ECHOE;  // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT, and SUSP

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control

  tty.c_oflag &= ~OPOST; // Raw output

  tcsetattr(serial_port, TCSANOW, &tty);

  printf("\n");
  int index = 0;
  while (1) {
    bytes_read = read(serial_port, &buffer, sizeof(buffer));
    if (bytes_read == 1) {
      if (buffer[0] == '\n') {
        buffer_f[index] = '\0';
        index = 0;
        // printf("Potentiometer Value: %s\n", buffer_f);
        match_input();
        printf("\n");
        continue;
      }
      buffer_f[index] = buffer[0];
      index++;
    }
  }

  close(serial_port);
  printf("Serial connection closed.\n");

  return 0;
}

void match_input() {
  int value;
  char pressed[] = {"p"};
  char released[] = {"r"};
  char up[] = {"u"};
  char down[] = {"d"};
  char left[] = {"l"};
  char rigth[] = {"r"};
  char v = buffer_f[0];
  if (v == 'X') {
    value = atoi(((char *)buffer_f) + 1);
    if (value > 580) { // rigth
      printf("%s", rigth);
    } else if (value < 400) { // left
      printf("%s", left);
    }
    return;
  }
  if (v == 'Y') {
    value = atoi(((char *)buffer_f) + 1);
    if (value > 580) { // up
      printf("%s", up);
    } else if (value < 400) { // down
      printf("%s", down);
    }
    return;
  }
  if (v == 'b') {
    switch (buffer_f[1]) {
    case 'p': // Presed
      printf("%s", pressed);
      break;
    case 'r': // relesed
      printf("%s", released);
      break;
    }
  }
}
