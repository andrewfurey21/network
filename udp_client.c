

#include <netdb.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

int main(int argc, char** argv) {
  
  assert(argc == 3);
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;

  struct addrinfo *peer_address;
  if (getaddrinfo(argv[1], argv[2], &hints, &peer_address) < 0) {
    printf("could not create internet address\n");
    exit(-1);
  }

  int peer = socket(peer_address->ai_family,
                    peer_address->ai_socktype,
                    peer_address->ai_protocol);

  if (peer < 0) {
    printf("could not create peer socket\n");
    exit(-1);
  }

  const char* message = "hello faggot\n";
  sendto(peer, message, strlen(message), 0, peer_address->ai_addr, peer_address->ai_addrlen);

  return 0;
}
