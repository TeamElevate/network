#include <arpa/inet.h>
#include <stdio.h>
#include <sys/poll.h>
#include <time.h>

#include "udp.h"
#include "beacon.h"
#include "peer.h"
#include "peers.h"

#define PING_MSG_SIZE 1
#define PING_PORT_NUMBER 9999
#define PING_INTERVAL    1  //  Once per second


int main(const int argc, const char* argv[]) {
  char ip[INET_ADDRSTRLEN] = { '\0' };
  const char* interface = NULL;
  if (argc > 1) {
    interface = argv[1];
  }

  udp_t *udp = udp_new(PING_PORT_NUMBER, interface);
  struct pollfd ufds[1];
  int ret;
  time_t  t0, t1;
  beacon_t beacon;
  uuid_t uuid;
  beacon_t recv;
  peers_t* peers = peers_new();

  uuid_generate(uuid);
  beacon_fill(&beacon, (uint8_t*)BEACON_PROTOCOL, BEACON_VERSION, uuid, htons(47473));

  ufds[0].fd = udp_handle(udp);
  ufds[0].events = POLLIN;

  t0 = time(NULL);
  t1 = time(NULL);

  struct sockaddr_in sockaddr;
  socklen_t si_len = sizeof(struct sockaddr_in);

  while (1) {
    t1 = time(NULL);
    if ((long)(t1 - t0) >= PING_INTERVAL) {
      udp_send(udp, (uint8_t*)(&beacon), sizeof(beacon_t));
      t0 = time(NULL);
    }
    ret = poll(ufds, 1, 200);
    if (ret == -1) {
      printf("Error: poll returned -1\n");
      break;
    } else if (ret == 0) {
      continue;
    }

    if (ufds[0].revents & POLLIN) {
      ret = udp_recv(udp, (uint8_t*)&recv, sizeof(beacon_t), &sockaddr, si_len);
      if (ret == sizeof(beacon_t) && beacon_check(&recv, (uint8_t*)BEACON_PROTOCOL, BEACON_VERSION)) {
        if (uuid_compare(uuid, recv.uuid) != 0) {
          peer_t* peer = peers_exist(peers, recv.uuid);
          if (!peer) {
            inet_ntop(sockaddr.sin_family, &sockaddr.sin_addr, ip, INET_ADDRSTRLEN);
            peer = peer_new(recv.uuid, ip, recv.port);
            printf("New Peer %s\n", peer_ip(peer));
            peers_add(peers, peer);
          }
          peer_is_alive(peer);
        }
      }
    }
    peers_check(peers);
  }


  udp_destroy(&udp);
  return 0;
}
