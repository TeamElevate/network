#ifndef __ELEVATE_PEERS__
#define __ELEVATE_PEERS__
#include <uuid/uuid.h>

#include "peer.h"

typedef struct _peers_t peers_t;

peers_t* peers_new();
void peers_destroy(peers_t** self_p);

peer_t* peers_exist(const peers_t* self, const uuid_t uuid);
int peers_add(peers_t* self, peer_t* peer);
peer_t* peers_remove(peers_t* self, const uuid_t uuid);
int peers_check(peers_t* self);
void peers_print(peers_t* self, FILE* stream);
int  peers_size(peers_t* self);

#endif
