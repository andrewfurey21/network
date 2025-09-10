
#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <errno.h>

#include <unistd.h>

int main(int argc, char** argv) {

  if (argc != 3) {
    printf("Error: must have 3 arguments. %d were found.\n", argc);
    exit(-1);
  }

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM; // TCP
  struct addrinfo *peer_address;
  if (getaddrinfo(argv[1], argv[2], &hints, &peer_address)) {
    printf("Error calling getaddrinfo()\n");
    exit(-1);
  }

  char address[100], port[100];
  getnameinfo(peer_address->ai_addr,
              peer_address->ai_addrlen,
              address, sizeof(address),
              port, sizeof(port),
              NI_NUMERICHOST);
  printf("%s:%s\n", address, port);


  int socket_peer = socket(peer_address->ai_family,
                           peer_address->ai_socktype,
                           peer_address->ai_protocol);

  if (socket_peer < 0) {
    printf("Error: could not create socket\n");
    exit(-1);
  }

  if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen)) {
    printf("couldn't connect\n");
    exit(-1);
  }

  freeaddrinfo(peer_address);
  printf("Connected.\n");

  while (true) {
    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(socket_peer, &reads);
    FD_SET(0, &reads);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0) {
      printf("select failed: %d\n", errno);
      exit(-1);
    }

    if (FD_ISSET(socket_peer, &reads)) {
      char read[4096];
      int bytes_received = recv(socket_peer, read, sizeof(read), 0);
      if (bytes_received < 1) {
        printf("Connection closed\n");
        break;
      }
      printf("received %d bytes: %.*s\n", bytes_received, bytes_received, read);
      printf("finished here\n");
      // printf("received %d bytes: %.*s\n", bytes_received, bytes_received, read);
    }

    if (FD_ISSET(0, &reads)) { // stdin
      char read[4096];
      if (!fgets(read, sizeof(read), stdin)) {
        printf("couldn't read stdin\n");
        exit(-1);
      }

      int bytes_sent = send(socket_peer, read, strlen(read), 0);
    }
  }

  close(socket_peer);

  return 0;
}
