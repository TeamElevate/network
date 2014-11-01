#include "udp.h"

#define PING_PORT_NUMBER 9999

int main(void) {
  udp_t *udp = udp_new(PING_PORT_NUMBER);

  udp_destory(&udp);
}
