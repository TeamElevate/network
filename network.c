#include <ifaddrs.h>
#include <string.h>

#include "network.h"
int find_my_ip(struct in_addr* addr, const char* interface_name) {
  struct ifaddrs* interfaces;
  int found = 0;
  if (getifaddrs(&interfaces) != 0) return -1;

  struct ifaddrs* interface = interfaces;

  while (interface) {
    if (!interface_name || (strcmp(interface->ifa_name, interface_name) == 0)) {
      if (interface->ifa_addr->sa_family == AF_INET) {
        *addr = ((struct sockaddr_in*)(interface->ifa_addr))->sin_addr;
        found++;
        if (interface_name) break;
      }
    }
    interface = interface->ifa_next;
  }
  freeifaddrs(interfaces);
  return found;
}
