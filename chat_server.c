
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

void upper(char* a, int l) {
  for (int i = 0; i < l; i++) {
    if (a[i] >= 'a' && a[i] <= 'z') {
      a[i] += 'A' - 'a';
    }
  }
}

int main() {
  const char* port = "8080";
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  struct addrinfo *bindaddress;
  if (getaddrinfo(0, port, &hints, &bindaddress)) {
    printf("error calling getaddrinfo\n");
    exit(-1);
  }

  // file descriptor to a network socket.
  int socket_listen = socket(bindaddress->ai_family,
                             bindaddress->ai_socktype,
                             bindaddress->ai_protocol);
  printf("port %s\n", port);

  int option = 1; // turn on an option.
  if (setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof option)) {
    printf("set sock opt failed\n");
    exit(-1);
  }

  if (socket < 0) { // file descriptor < 0 means error
    printf("couldn't create socket\n");
    exit(-1);
  }

  if (bind(socket_listen, bindaddress->ai_addr, bindaddress->ai_addrlen)) {
    printf("Couldn't bind socket\n");
    exit(-1);
  }

  freeaddrinfo(bindaddress);

  if (listen(socket_listen, 10)  < 0) {
    printf("couldn't listen\n");
    exit(-1);
  }

  fd_set master; // set of file descriptors. internally i'm guessing some sort of bitmask, on if file descriptor is included. a file descriptor is just a non negative integer given to an open file in the current process. if a negative number is returned, that means an error occurred.
  FD_ZERO(&master); // initializes the file descriptor set.
  FD_SET(socket_listen, &master); // adds socket_listen to the file descriptor set.

  int max = socket_listen;

  while (true) {
    fd_set reads = master; // make a copy of file descriptors.
    if (select(max + 1, &reads, 0, 0, 0) < 0) { // if == 0, timeout expired.
      // select sets any file descriptor in reads to 0, if it is not ready to be read from.
      printf("error: %d\n", errno);
      exit(-1);
    }

    for (int socket = 1; socket <= max; socket++) {
      if (FD_ISSET(socket, &reads)) { // read socket iff socket is ready to be read, if we didn't do this, calls to accept/recv would be blocked.
        if (socket == socket_listen) {
          struct sockaddr_storage client_address; // could be ipv4 or ipv6, which have different sizes. sockaddr_storage has enough padding to cast to it.
          socklen_t client_len = sizeof(client_address);
          int client = accept(socket_listen, (struct sockaddr *)&client_address, &client_len);
          if (client < 0) {
            printf("error with client\n");
            exit(-1);
          }

          FD_SET(client, &master);
          if (client > max) {
            max = client;
          }

          char address_buffer[100];
          getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, 100, 0, 0, NI_NUMERICHOST);
          printf("Connection with %s\n", address_buffer);
        } else {
          char read[4096];
          int received = recv(socket, read, sizeof(read), 0);

          printf("%.*s\n", received, read);

          if (received < 0) {
            FD_CLR(socket, &master);
            close(socket);
            continue;
          }

          for (int other = 1; other <= max; other++) {
            if (FD_ISSET(other, &master)) {
              if (other == socket || other == socket_listen) continue;
              send(other, read, received, 0);
            }
          }
        }
      }
    }
  }

  close(socket_listen);

  return 0;
}
