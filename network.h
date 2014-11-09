#ifndef __ELEVATE_NETWORK__
#define __ELEVATE_NETWORK__
#include <arpa/inet.h>

int find_my_ip(struct in_addr* addr, const char* interface_name);
#endif
