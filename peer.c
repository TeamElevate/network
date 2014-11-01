#include <assert.h>     /* assert */
#include <stdlib.h>     /* malloc */
#include <string.h>     /* strcpy */

#include "peer.h"

struct _peer_t {
  uuid_t   uuid;
  uint64_t expires_at;
  char ip[16];
  int  port;
};

peer_t* peer_new(uuid_t uuid, const char* ip, int port) {
  peer_t* self = (peer_t*)malloc(sizeof(peer_t));
  self->port = port;
  // @TODO: NOT SAFE
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

uint8_t* peer_uuid(peer_t* self) {
  assert(self);
  return self->uuid;
}

const char* peer_ip(peer_t* self) {
  assert(self);
  return self->ip;
}

int peer_port(peer_t* self) {
  assert(self);
  return self->port;
}

void peer_is_alive(peer_t* self) {
  assert(self);
  // @TODO
}
