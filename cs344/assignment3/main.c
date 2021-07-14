#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

/**
* Set up a sockaddr_in sturcture with the value of hostname and port passed as arguments to the program
*/

int main(int argc, char *argv[]) {
  int   portNumber;
  struct hostent* hostInfo;
  struct sockaddr_in address;

  // Check usage & args
  if (argc < 3) { 
    fprintf(stderr,"USAGE: %s hostname port\n", argv[0]);
    fprintf(stderr,"For example: %s oregonstate.edu 8080\n", argv[0]); 
    exit(0); 
  } 

  // Clear out the address struct
  memset((char*) &address, '\0', sizeof(address));

  // The address should be network capable
  address.sin_family = AF_INET;

  // Store the port number
  portNumber = atoi(argv[2]);
  address.sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  hostInfo = gethostbyname(argv[1]); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  // NOTE: The cast to (char*) is not needed. See Piazza post @153
  memcpy((char*) &address.sin_addr.s_addr, 
          hostInfo->h_addr_list[0],
          hostInfo->h_length);

  // Print the IP address in the socket address as a dotted decimal string
  printf("IP address for %s: %s\n", argv[1], inet_ntoa(address.sin_addr));

  // EXERCISE: Replace ... with code that prints the port number in the socket address in the variable address
  printf("Port number in the socket address: %d\n", ntohs(address.sin_port));

  return 0;
}