#include <assert.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <uuid/uuid.h>

#include "btle.h"
#include "beacon.h"
#include "discovery.h"

int main(int argc, const char* argv[]) {
  int i;

  int currentAdapterState;
  evt_le_meta_event *leMetaEvent;
  le_advertising_info *leAdvertisingInfo;
  char uuid_str[2048];

  fd_set rfds;
  struct timeval tv;
  int selectRetval;

  unsigned char hciEventBuf[HCI_MAX_EVENT_SIZE];
  int hciEventLen;

  int ret;

  beacon_t beacon;
  uuid_t uuid;
  struct in_addr addr;
  const char* interface = NULL;
 
  if (argc > 1) {
    interface = argv[1];
  }
  find_my_ip(&addr, interface);

  uuid_generate(uuid);
  beacon_fill(&beacon, BEACON_PROTOCOL, BEACON_VERSION, uuid, addr, 13377);

  btle_t* btle = btle_new();
  if (!btle) {
    printf("Error: Could not init btle\n");
    return -1;
  }

  ret = btle_set_filter(btle);
  assert(ret == 0);

  ret = btle_set_adv_data(btle, &beacon, sizeof(beacon));
  assert(ret == 0);

  //turn on le advertise
  ret = btle_start_adv(btle);
  ret = btle_start_scan(btle);

  while (1) {
    FD_ZERO(&rfds);
    FD_SET(btle_sock(btle), &rfds);

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    currentAdapterState = btle_dev_is_up(btle);
    if (!currentAdapterState) {
      printf("Powered Off\n");
      break;
    }

    selectRetval = select(btle_sock(btle) + 1, &rfds, NULL, NULL, &tv);

    if (selectRetval >= 1) {
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
        inet_ntop(AF_INET, &peer->addr, uuid_str);
        printf("Found Peer: %s\n", uuid_str);
      }
    }
    

  }
  btle_destroy(&btle);
}
