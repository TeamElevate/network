#include <stdio.h>
#include <sys/poll.h>
#include <time.h>

#include "udp.h"

#define PING_MSG_SIZE 1
#define PING_PORT_NUMBER 9999
#define PING_INTERVAL    1  //  Once per second

int main(void) {
  udp_t *udp = udp_new(PING_PORT_NUMBER, NULL);
  uint8_t buffer[PING_MSG_SIZE];
  struct pollfd ufds[1];
  int ret;
  time_t  t0, t1;

  buffer[0] = '!';

  ufds[0].fd = udp_handle(udp);
  ufds[0].events = POLLIN;

  t0 = time(NULL);
  t1 = time(NULL);

  udp_send(udp, buffer, PING_MSG_SIZE);
  udp_send(udp, buffer, PING_MSG_SIZE);
  while (1) {
    t1 = time(NULL);
    if ((long)(t1 - t0) >= PING_INTERVAL) {
      udp_send(udp, buffer, PING_MSG_SIZE);
      t0 = time(NULL);
    }
    ret = poll(ufds, 1, 200);
    if (ret == -1) {
      printf("Error: poll returned -1\n");
    } else if (ret == 0) {
      // timeout
    } else {
      if (ufds[0].revents & POLLIN) {
        udp_recv(udp, buffer, PING_MSG_SIZE);
        printf("Recieved: %c\n", buffer[0]);
      } else {
        printf("Something else\n");
      }
    }
  }


  udp_destroy(&udp);
}
