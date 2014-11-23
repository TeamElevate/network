#include <assert.h>     /* assert */
#include <stdlib.h>     /* malloc */
#include <string.h>     /* strcpy */
#include <time.h>       /* time_t */

#include "peer.h"

static const long SECONDS_TILL_MISSING = 1;

struct _peer_t {
  uuid_t   uuid;
  time_t last_updated;
  char ip[16];
  int  port;
};

peer_t* peer_new(const uuid_t uuid, const char* ip, int port) {
  assert(ip);

  peer_t* self = (peer_t*)malloc(sizeof(peer_t));
  self->port = port;
  strcpy(self->ip, ip);
  memcpy(self->uuid, uuid, sizeof(uuid_t));
  return self;
}

void peer_destroy(peer_t** self_p) {
  assert(self_p);
  if (*self_p) {
    peer_t* self = *self_p;
    free(self);
    *self_p = NULL;
  }
}

const uint8_t* peer_uuid(const peer_t* self) {
  assert(self);
  return self->uuid;
}

const char* peer_ip(const peer_t* self) {
  assert(self);
  return self->ip;
}

int peer_port(const peer_t* self) {
  assert(self);
  return self->port;
}

void peer_is_alive(peer_t* self) {
  assert(self);
  self->last_updated = time(NULL);
}

int is_peer_missing(const peer_t* self) {
  assert(self);
  return (time(NULL) - self->last_updated) > SECONDS_TILL_MISSING;
}
