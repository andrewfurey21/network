#include <ifaddrs.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>

int main(void) {

  struct ifaddrs *addresses;

  if (getifaddrs(&addresses) == -1) {
    printf("Could not get  ifaddrs");
    return -1;
  }

  struct ifaddrs *address = addresses;
  while (address) {
    const int family = address->ifa_addr->sa_family;
    if (family == AF_INET || family == AF_INET6) {
      printf("%s, ", address->ifa_name);
      printf("%s, ", family == AF_INET ? "IPv4" : "IPv6");

      const int family_size = family == AF_INET ?
        sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
      // Gets the IP address number
      char ap[100];
      getnameinfo(address->ifa_addr, family_size, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
      printf("%s", ap);
      printf("\n");
    }
    address = address->ifa_next;
  }
  freeifaddrs(addresses);
  return 0;
}
