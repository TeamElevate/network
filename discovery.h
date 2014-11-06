#ifndef __ELEVATE_DISCOVERY__
#define __ELEVATE_DISCOVERY__
#include <netinet/in.h>

typedef struct _discovery_t discovery_t;
typedef void (*callback_t)(void* data);

discovery_t* discovery_new(const char* interface, callback_t add_cb, callback_t rem_cb);
void discovery_destroy(discovery_t** self_p);
void discovery_start(discovery_t* self);
void discovery_stop(discovery_t* self);

int  find_my_ip(struct in_addr* addr);
#endif
