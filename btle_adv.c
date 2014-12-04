#include <errno.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "btle.h"
#include "beacon.h"
#include "network.h"

int main(const int argc, char* const * argv) {
  // getopt variables
  int index;
  int c;

  int start = 0;
  int stop = 0;
  beacon_t beacon;
  uuid_t uuid;
  struct in_addr addr = { 0 };
  const char* interface = NULL;
  int ret;
  pid_t pid, sid;

  opterr = 0;

  while ((c = getopt(argc, argv, "sei:")) != -1) {
    switch (c) {
      case 's':
        start = 1;
        break;

      case 'e':
        stop = 1;
        break;

      case 'i':
        interface = optarg;
        break;

      case '?':
        if (optopt == 'i') {
          printf("Error: -i requires an arguement\n");
        } else {
          printf("Error: Unknown arguement %c\n", optopt);
        }
        return -1;

      default:
        abort();
    }
  }

  if (start == stop) {
    fprintf(stderr, "Only specify start xor stop\n");
    return -2;
  }

  btle_t* btle = btle_new();

  if (!btle) {
    printf("Error Starting Bluetooth\n");
    return -1;
  }
 
  if (start) {

    if (find_my_ip(&addr, interface) <= 0) {
      inet_pton(AF_INET, "127.0.0.1", &addr);
    }

    uuid_generate(uuid);
    beacon_fill(&beacon, BEACON_PROTOCOL, BEACON_VERSION, uuid, addr, 13377);

    ret = btle_set_adv_data(btle, &beacon, sizeof(beacon));
    if (ret != 0) {
      printf("Error Setting Adv Data: %d\n", ret);
      return -1;
    }
    ret = btle_start_adv(btle);
    if (ret != 0) {
      printf("Error Starting Advertising: %d\n", ret);
      return -1;
    }
  } else if (stop) {
    ret = btle_stop_adv(btle);
    if (ret != 0) {
      printf("Error Stopping Advertising: %d\n", ret);
      return -1;
    }
  }
  return 0;
}
