#include <assert.h>
#include <string.h>

#include "beacon.h"

void beacon_fill(beacon_t* self, const uint8_t* protocol, uint8_t version, const uuid_t uuid, const struct in_addr addr,
uint16_t port) {
  assert(self);
  assert(protocol);
  int i;

  for (i = 0; i < 3; i++) {
    self->protocol[i] = protocol[i];
  }

  self->version = version;
  memcpy(self->uuid, uuid, sizeof(uuid_t));
  self->addr = addr;
  self->port = port;
}

int beacon_check(const beacon_t* self, const uint8_t* protocol, uint8_t version) {
  assert(self);
  assert(protocol);
  int i;

  for (i = 0; i < 3; i++) {
    if (self->protocol[i] != protocol[i]) return 0;
  }

  if (self->version != version) return 0;
  
  return 1;
}
