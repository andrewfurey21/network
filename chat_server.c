
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

  int socket_listen = socket(bindaddress->ai_family,
                             bindaddress->ai_socktype,
                             bindaddress->ai_protocol);
  printf("port %s\n", port);

  int option = 1; // turn on
  if (setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof option)) {
    printf("set sock opt failed\n");
    exit(-1);
  }

  if (socket < 0) {
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

  fd_set master;
  FD_ZERO(&master);
  FD_SET(socket_listen, &master);

  int max = socket_listen;

  while (true) {
    fd_set reads;
    reads = master;
    if (select(max + 1, &reads, 0, 0, 0) < 0) {
      printf("error: %d\n", errno);
      exit(-1);
    }

    for (int socket = 1; socket <= max; socket++) {
      if (FD_ISSET(socket, &reads)) {
        if (socket == socket_listen) {
          struct sockaddr_storage client_address;
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

          upper(read, 1);
            // read[j] = toupper(read[j]);
          send(socket, read, strlen(read), 0);
        }
      }
    }
  }

  close(socket_listen);

  return 0;
}
