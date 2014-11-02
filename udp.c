#include <assert.h>     /* assert */
#include <arpa/inet.h>  /* htons */
#include <ifaddrs.h>    /* getifaddrs, freeifaddrs */
#include <sys/socket.h> /* socket, setsockopt */
#include <netinet/in.h> /* sockaddr_in */
#include <stdlib.h>     /* malloc */
#include <stdio.h>      /* printf */
#include <string.h>     /* strcmp */
#include <unistd.h>     /* close */
#include "udp.h"


struct _udp_t {
  int handle;
  int port_nbr;
  struct sockaddr_in address;
  struct sockaddr_in broadcast;
};

udp_t* udp_new(int port_nbr, const char* interface_name) {
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
  sockaddr.sin_port   = htons(self->port_nbr);
  sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  assert(bind(self->handle, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) != -1);

  struct ifaddrs* interfaces;
  if (getifaddrs(&interfaces) == 0) {
    struct ifaddrs* interface = interfaces;
    while (interface) {
      if (interface->ifa_addr->sa_family == AF_INET) {
        self->address   = *(struct sockaddr_in*) interface->ifa_addr;
        self->broadcast = *(struct sockaddr_in*) interface->ifa_broadaddr;
        self->broadcast.sin_port = htons(self->port_nbr);
        if (interface_name && ( strcmp(interface_name, interface->ifa_name) == 0)) {
          break;
        }
      }
      interface = interface->ifa_next;
    }
    assert(!interface_name || interface);
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

int udp_handle(const udp_t* self) {
  assert(self);
  return self->handle;
}

void udp_send(udp_t* self, const uint8_t* buffer, size_t len) {
  assert(self);
  assert(sendto(self->handle, buffer, len, 0, (struct sockaddr*)&self->broadcast, sizeof(struct sockaddr_in)) != -1);
}

ssize_t udp_recv(udp_t* self, uint8_t *buffer, size_t len, struct sockaddr_in* sender, socklen_t si_len) {
  assert(self);
  assert(sender);

  ssize_t size = recvfrom(self->handle, buffer, len, 0, (struct sockaddr*)sender, &si_len);
  assert(size != -1);

  return size;
}
