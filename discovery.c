#include <arpa/inet.h>
#include <assert.h>
#include <ifaddrs.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/poll.h>
#include <time.h>

#include "udp.h"
#include "beacon.h"
#include "peer.h"
#include "peers.h"

#include "discovery.h"

#define PING_PORT_NUMBER 9999
#define PING_INTERVAL    1  //  Once per second

typedef struct {
  const char* interface;
  callback_t add_peer_cb;
  callback_t remove_peer_cb;
} discovery_opts_t;

struct _discovery_t {
  pthread_t thread;
  int started;
  discovery_opts_t opts;
};

static void* run_discovery(void* data);

discovery_t* discovery_new(const char* interface, callback_t add_cb, callback_t rem_cb) {
  discovery_t* self = malloc(sizeof(discovery_t));

  self->started = 0;
  self->opts.interface = interface;
  self->opts.add_peer_cb = add_cb;
  self->opts.remove_peer_cb = rem_cb;

  return self;
}

void discovery_destroy(discovery_t** self_p) {
  assert(self_p);
  if (*self_p) {
    discovery_t* self = *self_p;
    if (self->started) {
      discovery_stop(self);
    }
    free(self);
    *self_p = NULL;
  }
}

void discovery_start(discovery_t* self) {
  assert(self);

  self->started = 1;
  pthread_create(&self->thread, NULL, run_discovery, &self->opts);
}

void discovery_stop(discovery_t* self) {
  assert(self);

  pthread_cancel(self->thread);
}

int find_my_ip(struct in_addr* addr) {
  struct ifaddrs* interfaces;
  int found = -1;
  if (getifaddrs(&interfaces) != 0) return -1;

  struct ifaddrs* interface = interfaces;

  while (interface) {
    if (interface->ifa_addr->sa_family == AF_INET) {
      *addr = ((struct sockaddr_in*)(interface->ifa_addr))->sin_addr;
      found = 0;
    }
    interface = interface->ifa_next;
  }
  freeifaddrs(interfaces);
  return found;
}

static void* run_discovery(void* data) {
  discovery_opts_t* opts = (discovery_opts_t*)data;
  char ip[INET_ADDRSTRLEN] = { '\0' };
  const char* interface = opts->interface;

  udp_t *udp = udp_new(PING_PORT_NUMBER, interface);
  assert(udp);

  struct pollfd ufds[1];
  int ret;
  time_t  t0, t1;
  beacon_t beacon;
  uuid_t uuid;
  beacon_t recv;
  peers_t* peers = peers_new();
  struct in_addr addr;

  find_my_ip(&addr);

  uuid_generate(uuid);
  beacon_fill(&beacon, (uint8_t*)BEACON_PROTOCOL, BEACON_VERSION, uuid, addr, htons(47473));

  ufds[0].fd = udp_handle(udp);
  ufds[0].events = POLLIN;

  t0 = time(NULL);
  t1 = time(NULL);

  struct sockaddr_in sockaddr;
  socklen_t si_len = sizeof(struct sockaddr_in);

  while (1) {
    t1 = time(NULL);
    if ((long)(t1 - t0) >= PING_INTERVAL) {
      ret = udp_send(udp, (uint8_t*)(&beacon), sizeof(beacon_t));
      assert(ret == sizeof(beacon_t));
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
            peers_add(peers, peer);
            opts->add_peer_cb((void*)peer);
          }
          peer_is_alive(peer);
        }
      }
    }
    peers_check(peers);
  }

  peers_destroy(&peers);
  udp_destroy(&udp);
  return NULL;
}
