// #include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <netdb.h>
// #include <unistd.h>
// #include <errno.h>

#include <time.h>
#include <stdio.h>

int main(void) {

  time_t timer;
  time(&timer);

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));

  hints.ai_family = af_inet;
  hints.ai_socktype = sock_stream;
  hints.ai_flags = ai_passive;

  struct addrinfo *bindaddress;
  int a = getaddrinfo(0, "8080", &hints, &bindaddress);
  printf("%d\n", a);




  return 0;
}
