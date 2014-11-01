#include "udp.h"

struct _udp_t {
  int handle;
  int port_nbr;
  struct sockaddr_in address;
  struct sockaddr_in broadcast;
};

udp_t* udp_new(int port_nbr) {
  udp_t* self = (udp_t*) malloc(sizeof(udp_t));
  self->port_nbr = port_nbr;

  // Create udp socket
  self->handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  assert(self->handle > 0);

  // Set up socket for broadcast
  int on = 1;
  assert(setsockopt(self->handle, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) != -1);

  // Allow multiple processes to bind to socket
  // incoming messages will go to each process
  assert(setsockopt(self->handle, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != -1);

  struct sockaddr_in sockaddr = { 0 };
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port   = htons(self->portnbr);
  sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  assert(bind(self->handle, &sockaddr, sizeof(sockaddr)) != -1);

  struct ifaddrs* interfaces;
  if (getifaddrs(&interfaces) == 0) {
    struct ifaddrs* interface = interfaces;
    while (interface) {
      // @TODO: Don't rely on last interface being correct
      if (interface->ifa_addr->sa_family == AF_INET) {
        self->address   = *(struct sockaddr_in*) interface->ifa_addr;
        self->broadcast = *(struct sockaddr_in*) interface->ifa_broadaddr;
        self->broadcast.sin_port = htons(self->port_nbr);
      }
      interface = interface->ifa_next;
    }
  }
  freeifaddrs(interfaces);
  return self;
}

void udp_destroy(udp_t** self_p) {
  assert(self_p);
  if (*self_p) {
    udp_t* self = *self_p;
    close(self->handle);
    free(self);
    *self_p = NULL;
  }
}

int udp_handle(udp_t* self) {
  assert(self);
  return self->handle;
}

void udp_send(udp_t* self, uint8_t buffer, size_t len) {
  assert(self);
  assert(sendto(self->handle, buffer, len, 0, &self->broadcast, sizeof(struct sockaddr_in)) != -1);
}

ssize_t udp_recv(udp_t* self, uint8_t buffer, size_t len) {
  assert(self);

  struct sockaddr_in sockaddr;
  socklen_t si_len = sizeof(struct sockaddr_in);

  ssize_t size = recvfrom(self->handle, buffer, length, 0, &sockaddr, &si_len);
  assert(size != -1);

  return size;
}
