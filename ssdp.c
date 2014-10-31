#include "ssdp.h"

#include <assert.h>      /* assert */
#include <arpa/inet.h>   /* pton */
#include <string.h>      /* memset */
#include <netinet/in.h>  /* struct sockaddr_in */
#include <stdio.h>       /* perror */
#include <stdlib.h>      /* exit */
#include <sys/socket.h>  /* socket */

const char * const SSDP_MULTICAST_ADDR = "239.255.255.250";
const int          SSDP_MULTICAST_PORT = 1900;

int ssdp_announce() {
  struct sockaddr_in addr;
  int sockd, cnt;
  int ret;
  struct ip_mreq mreq;
  char* message = "Hello, World!";

  // Create a UDP socket
  if ( (sockd=socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
    perror("socket");
    exit(1);
  }

  // Set up destination address
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  ret = inet_pton(AF_INET, SSDP_MULTICAST_ADDR, &(addr.sin_addr));
  assert(ret == 1);
  addr.sin_port = htons(SSDP_MULTICAST_PORT);

  // Send the annoucement
  ret = sendto(sockd, message, sizeof(message), 0, (struct sockaddr*) &addr, sizeof(addr));
  if (ret < 0) {
    perror("sendto");
    exit(1);
  }
  return 0;
}

int ssdp_listen() {

}

int main() {
  return ssdp_announce();
}
