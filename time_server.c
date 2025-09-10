#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>

#include <time.h>
#include <unistd.h>

int main(void) {
  const int queue_limit = 10;
  const char* port = "8070";
  const int version = AF_INET6; // ipv4

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

  int option = 0;
  if(setsockopt(listen_descriptor, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&option, sizeof(option))) {
    printf("Error: setsockopt failed\n");
    return -1;
  }

  option = 1;
  if(setsockopt(listen_descriptor, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(option))) {
    printf("Error: setsockopt failed\n");
    return -1;
  }

  int bind_error = bind(listen_descriptor, bindaddress->ai_addr, bindaddress->ai_addrlen);
  if (bind_error != 0) {
    printf("Error: couldn't bind socket to internet address. %d\n", errno);
    return -1;
  }

  if (listen(listen_descriptor, queue_limit) < 0) {
    printf("Error: listening failed.\n");
    return -1;
  }

  struct sockaddr_storage client_address;
  socklen_t client_len = sizeof(client_address);
  int socket_client = accept(listen_descriptor, (struct sockaddr*)&client_address, &client_len);

  if (socket_client < 0) {
    printf("Error: accept failed.\n");
    return -1;
  }

  char address_buffer[100];
  getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
  printf("Connection made with %s\n", address_buffer);

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

  freeaddrinfo(bindaddress);

  return 0;
}
