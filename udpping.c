#include <stdio.h>

#include "peer.h"
#include "discovery.h"

void add_peer(void* data) {
  peer_t* peer = (peer_t*)data;
  printf("Found Peer: %s\n", peer_ip(peer));
}

int main(const int argc, const char* argv[]) {
  const char* interface = NULL;
  if (argc > 1) {
    interface = argv[1];
  }

  discovery_t* dis = discovery_new(interface, add_peer,NULL);
  discovery_start(dis);
  
  while (1) 
    ;

  return 0;
}
