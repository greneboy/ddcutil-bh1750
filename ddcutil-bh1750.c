#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>

//....
#include "ddc-0x10.h"

//....
#define THRESHOLD 8.0 // Set the treshold to trigger ddcutil.

//....
#ifdef DEBUG
  #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(...) do {} while (0)
#endif

int main() {
  // ls /dev/serial/by-id, and find your arduino from there
  int serial_port = open("/dev/serial/by-id/usb-Arduino_LLC_Arduino_Leonardo-if00", O_RDWR);

  // Create new termios struct, we call it 'tty' for convention
  struct termios tty;

  // Read in existing settings, and handle any error
  if(tcgetattr(serial_port, &tty) != 0) {
      DEBUG_PRINT("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  // Set some tty cflags
  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO; // Disable echo
  tty.c_lflag &= ~ECHOE; // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG; // Disable INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

  // Apply the cflags
  tcflush(serial_port, TCIFLUSH);
  tcsetattr(serial_port, TCSANOW, &tty);

  // Set baud rates
  cfsetispeed(&tty, B9600);
  cfsetospeed(&tty, B9600);

  float last_value = 0.0;
  int first_read = 1;

  // This code is synchronous, I don't bother writing synchronous code, would be useless unless you're handling multiple serial_port's and other headers
  while (1) {

    // Perform a "handshake" to the serial out by sending a carriage return (\r)
    unsigned char msg[] = { '\r' };
    write(serial_port, msg, sizeof(msg));

    char read_buf[32];
    int total = 0;
    char c;

    // This is what makes it synchronous...
    // Block and wait for the serial_port to return \n
    while (total < sizeof(read_buf) - 1) {
      int num_bytes = read(serial_port, &c, 1);
      if (num_bytes <= 0) break;

      read_buf[total++] = c;
      if (c == '\n') break;
    }

    read_buf[total] = '\0';

    // serial_port returns "0d 0a" (\r\n) at the end of the readable buffer, instead trim "0d 0a" (\r\n). Just in case for clean data lol
    read_buf[strcspn(read_buf, "\r\n")] = '\0';

    // If the bytes -1 return stderr. Usually happens when serial_port is unplugged during this whole while() loop
    if (total < 0) {
      DEBUG_PRINT("Error reading: %s", strerror(errno));
      return 1;
    }

    float lux = atof(read_buf);

    if (first_read || fabs(lux - last_value) >= THRESHOLD) {

      // Simple logic
      if (lux > 10) {
        set_brightness_all_displays(100);
      }
      else {
        set_brightness_all_displays(60);
      }

      printf("%d BYTE: MAJOR: %.2f -> %.2f\n", total, last_value, lux);
      last_value = lux;
      first_read = 0;

    } else {

      DEBUG_PRINT("%d BYTE: MINOR: %.2f -> %.2f\n", total, last_value, lux);

    }
    sleep(1);

  }

  close(serial_port);
  return 0; // success

};
