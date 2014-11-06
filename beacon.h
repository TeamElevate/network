#ifndef __ELEVATE_BEACON__
#define __ELEVATE_BEACON__
#include <inttypes.h>
#include <netinet/in.h>
#include <uuid/uuid.h>  /* uuid_t */

static const char * const BEACON_PROTOCOL = "ZRE";
static const int BEACON_VERSION           = 0x01;

typedef struct {
    uint8_t        protocol[3];
    uint8_t        version;
    uuid_t         uuid;
    struct in_addr addr;
    uint16_t       port;
} beacon_t;

void beacon_fill(beacon_t* self, const uint8_t* protocol, uint8_t version, const uuid_t uuid, const struct in_addr addr, uint16_t port);
int beacon_check(const beacon_t* self, const uint8_t* protocol, uint8_t version);
#endif
