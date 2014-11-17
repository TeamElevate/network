#include <arpa/inet.h>
#include <assert.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <errno.h>
#include <sys/file.h>
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <uuid/uuid.h>

#include "beacon.h"
#include "btle.h"
#include "peer.h"
#include "peers.h"

int main(int argc, const char* argv[]) {
  FILE* fp;
  int i;

  int currentAdapterState;
  evt_le_meta_event *leMetaEvent;
  le_advertising_info *leAdvertisingInfo;
  char addr_str[INET_ADDRSTRLEN];

  struct pollfd ufds[1];

  unsigned char hciEventBuf[HCI_MAX_EVENT_SIZE];
  int hciEventLen;

  int rc;

  if (argc != 2) {
    printf("usage: ./btle_sniff outputfile\n");
    return -1;
  }

  peers_t* peers = peers_new();
  btle_t* btle = btle_new();
  if (!btle) {
    printf("Error: Could not init btle\n");
    return -1;
  }

  rc = btle_set_filter(btle);
  assert(rc == 0);

  rc = btle_start_scan(btle);

  // Setup Poll
  ufds[0].fd = btle_sock(btle);
  ufds[0].events = POLLIN;

  while (1) {
    currentAdapterState = btle_dev_is_up(btle);
    if (!currentAdapterState) {
      printf("Lost Bluetooth Adapter\n");
      break;
    }

    rc = poll(ufds, 1, 200);

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
        beacon_t* beacon = (beacon_t*)leAdvertisingInfo->data;
        inet_ntop(AF_INET, &beacon->addr, addr_str, INET_ADDRSTRLEN);
        peer_t* peer = peer_new(beacon->uuid, addr_str, beacon->port);
        if (!peers_exist(peers, beacon->uuid)) {
          peers_add(peers, peer);
          peer_is_alive(peer);
        } else {
          peer_is_alive(peer);
          peer_destroy(&peer);
        }
      }
    }
    peers_check(peers);
    if (peers_size(peers) > 0) {

      fp = fopen(argv[1], "w+");
      rc = flock(fileno(fp), LOCK_EX);
      peers_print(peers, fp);
      rc = flock(fileno(fp), LOCK_UN);
      fclose(fp);
    }
  }
  btle_stop_scan(btle);
  btle_destroy(&btle);
  return 0;
}
