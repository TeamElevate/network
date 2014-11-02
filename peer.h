#ifndef __ELEVATE_PEER__
#define __ELEVATE_PEER__
#include <inttypes.h>   /* uint8_t */
#include <uuid/uuid.h>  /* uuid_t */
typedef struct _peer_t peer_t;

peer_t* peer_new(uuid_t uuid, const char* ip, int port);
void    peer_destroy(peer_t** self_p);

const char* peer_ip(const peer_t* self);
int peer_port(const peer_t* self);
const uint8_t* peer_uuid(const peer_t* self);
void peer_is_alive(peer_t* self);
int is_peer_missing(const peer_t*  self);
#endif
