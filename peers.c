#include <assert.h> /* assert */
#include <stdlib.h> /* malloc */
#include <stdio.h>  /* fprintf, FILE */

#include "peers.h"

static const int MAX_NUM_PEERS = 5;

struct _peers_t {
  peer_t** peers;
  int num_peers;
};

peers_t* peers_new() {
  peers_t* self = malloc(sizeof(peers_t));
  self->peers = malloc(sizeof(peer_t*) * MAX_NUM_PEERS);
  self->num_peers = 0;
  return self;
}

void peers_destroy(peers_t** self_p) {
  int i;
  assert(self_p);
  if (*self_p) {
    peers_t* self = *self_p;
    for (i = 0; i < self->num_peers; i++) {
      peer_destroy(&self->peers[i]);
    }
    free(self->peers);
    free(self);
    *self_p = NULL;
  }
}

peer_t* peers_exist(const peers_t* self, const uuid_t uuid) {
  int i;
  for (i = 0; i < self->num_peers; i++) {
    if (uuid_compare(peer_uuid(self->peers[i]), uuid) == 0) {
      return self->peers[i];
    }
  }
  return NULL;
}

int peers_add(peers_t* self, peer_t* peer) {
  if (peers_exist(self, peer_uuid(peer))) return 0;

  if (self->num_peers == MAX_NUM_PEERS) return -1;
  self->peers[self->num_peers++] = peer;
  return 1;
}

peer_t* peers_remove(peers_t* self, const uuid_t uuid) {
  peer_t* victim = NULL;
  int i = 0;
  for (i = 0; i < self->num_peers; i++) {
    if (uuid_compare(peer_uuid(self->peers[i]), uuid) == 0) {
      break;
    }
  }
  if (i == self->num_peers) {
    return NULL;
  }
  victim = self->peers[i];
  for ( ; i < self->num_peers - 1; i++) {
    self->peers[i] = self->peers[i + 1];
  }
  self->num_peers--;
  return victim;
}

void peers_check(peers_t* self) {
  int i;
  for (i = 0; i < self->num_peers; i++) {
    if (is_peer_missing(self->peers[i])) {
      peers_remove(self, peer_uuid(self->peers[i]));
      i = -1;
    }
  }
}

void peers_print(peers_t* self, FILE* stream) {
  int i;
  for (i = 0; i < self->num_peers; i++) {
    fprintf(stream, "%s:%d\n", peer_ip(self->peers[i]), peer_port(self->peers[i]));
  }
}

int  peers_size(peers_t* self) {
  return self->num_peers;
}
