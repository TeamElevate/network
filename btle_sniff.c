#include <arpa/inet.h>
#include <assert.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <uuid/uuid.h>

#include "beacon.h"
#include "btle.h"

int main(int argc, const char* argv[]) {
  int i;

  int currentAdapterState;
  evt_le_meta_event *leMetaEvent;
  le_advertising_info *leAdvertisingInfo;
  char addr_str[INET_ADDRSTRLEN];

  struct pollfd ufds[1];

  unsigned char hciEventBuf[HCI_MAX_EVENT_SIZE];
  int hciEventLen;

  int ret;

  btle_t* btle = btle_new();
  if (!btle) {
    printf("Error: Could not init btle\n");
    return -1;
  }

  ret = btle_set_filter(btle);
  assert(ret == 0);

  ret = btle_start_scan(btle);

  // Setup Poll
  ufds[0].fd = btle_sock(btle);
  ufds[0].events = POLLIN;

  while (1) {
    currentAdapterState = btle_dev_is_up(btle);
    if (!currentAdapterState) {
      printf("Lost Bluetooth Adapter\n");
      break;
    }

    ret = poll(ufds, 1, 200);

    if (ufds[0].revents & POLLIN) {
      hciEventLen = read(btle_sock(btle), hciEventBuf, sizeof(hciEventBuf));
      leMetaEvent = (evt_le_meta_event *)(hciEventBuf + (1 + HCI_EVENT_HDR_SIZE));
      hciEventLen -= (1 + HCI_EVENT_HDR_SIZE);
      /* Event is not a LE advertising event */
      if (leMetaEvent->subevent != 0x02) {
        continue;
      }
      leAdvertisingInfo = (le_advertising_info *)(leMetaEvent->data + 1);

      if (memcmp(BEACON_PROTOCOL, leAdvertisingInfo->data, 3) == 0) {
        beacon_t* peer = (beacon_t*)leAdvertisingInfo->data;
        inet_ntop(AF_INET, &peer->addr, addr_str, INET_ADDRSTRLEN);
        printf("Found Peer: %s\n", addr_str);
      }
    }
  }
  btle_stop_scan(btle);
  btle_destroy(&btle);
  return 0;
}
