#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>

#include <time.h>
#include <unistd.h>

int main(void) {
  const int queue_limit = 10;
  const char* port = "8080";
  const int version = AF_INET; // ipv4

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));

  hints.ai_family = version;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  struct addrinfo *bindaddress;
  if (getaddrinfo(0, port, &hints, &bindaddress) != 0) {
    printf("Error: couldn't create an internet address.\n");
    return -1;
  }

  int listen_descriptor = socket(bindaddress->ai_family, bindaddress->ai_socktype, bindaddress->ai_protocol);
  if (listen_descriptor < 0) {
    printf("Error: couldn't create a socket.\n");
    return -1;
  }

  int bind_error = bind(listen_descriptor, bindaddress->ai_addr, bindaddress->ai_addrlen);
  if (bind_error) {
    printf("Error: couldn't bind socket to internet address.\n");
    return -1;
  }

  if (listen(listen_descriptor, queue_limit) < 0) {
    printf("Error: listening failed.\n");
    return -1;
  } else {
    struct sockaddr_in *addr = (struct sockaddr_in*)bindaddress->ai_addr;
    const int max_len = 100;
    char ap[max_len];
    inet_ntop(version, addr, ap, max_len);
    printf("Listening at %s:%s\n", ap, port);
    fflush(stdout);
  }

  struct sockaddr_storage client_address;
  socklen_t client_len = sizeof(client_address);
  int socket_client = accept(listen_descriptor, (struct sockaddr*)&client_address, &client_len);

  if (socket_client < 0) {
    printf("Error: accept failed.\n");
    return -1;
  }

  printf("Connection made.\n");
  char request[1024];
  int bytes_received = recv(socket_client, request, 1024, 0);
  printf("Recevied %d bytes.\n", bytes_received);
  printf("%.*s", bytes_received, request);

  printf("Sending response:\n");
  const char* response =
    "HTTP/1.1 200 OK\r\n"
    "Connection: close\r\n"
    "Content-Type: text/plain\r\n\r\n"
    "Local time is: ";

  int bytes_sent = send(socket_client, response, strlen(response), 0); // should make sure that all bytes get sent.
  printf("Sent %d bytes\n", bytes_sent);

  time_t timer;
  time(&timer);

  char* timestr = ctime(&timer);
  printf("%s\n", timestr);
  bytes_sent = send(socket_client, timestr, strlen(timestr), 0);
  printf("Sent %d bytes\n", bytes_sent);

  close(socket_client);
  close(listen_descriptor);

  return 0;
}
