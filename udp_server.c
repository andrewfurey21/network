
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  struct addrinfo *bindaddress;
  if (getaddrinfo(0, "8080", &hints, &bindaddress) < 0) {
    printf("could not get addr info\n");
    exit(-1);
  }

  int listen_socket = socket(bindaddress->ai_family, bindaddress->ai_socktype, bindaddress->ai_protocol);

  if (listen_socket < 0) {
    printf("could not create socket\n");
    exit(-1);
  }

  if (bind(listen_socket, bindaddress->ai_addr, bindaddress->ai_addrlen) < 0) {
    printf("could not bind socket\n");
    exit(-1);
  }

  freeaddrinfo(bindaddress);

  struct sockaddr_storage client_address;
  socklen_t client_len;
  char buffer[4096];
  int bytes_received = recvfrom(listen_socket,
                                buffer,
                                sizeof(buffer),
                                0,
                                (struct sockaddr *)&client_address,
                                &client_len);

  printf("received %d bytes: %.*s\n", bytes_received, bytes_received, buffer);
  close(listen_socket);
  return 0;
}
