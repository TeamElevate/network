#ifndef __ELEVATE_UDP__
#define __ELEVATE_UDP__
typedef struct _udp_t udp_t;

// Constructor
udp_t*  udp_new(int port_nbr);
// Destructor
void    udp_destroy(udp_t **self_p);
int     udp_handle(udp_t* self);
void    udp_send(udp_t *self, uint8_t *buffer, size_t len);
ssize_t udp_recv(udp_t *self, uint8_t buffer, size_t len);
#endif
