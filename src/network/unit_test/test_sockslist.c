/**
 * File: test_sockslit.c
 * Author: Ethan Gordon
 *
 * SocksList Unit Tests
 **/

#define TEST_CAP 2

#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "../sockslist.h"

void verify(int expression, char* message) {

  static int x = 0;

  printf("Test %d: %s...", x, message);

  if(expression)
    printf("passed.\n");
  else
    printf("FAILED.\n");

  fflush(stdout);
  x++;
}

int main(void) {
  int test;
  int i;
  char* buf[14 * 1024];

  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;  // will point to the results
  struct sockaddr *dummy_socket;

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

  if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return 1;
  }

  dummy_socket = servinfo->ai_addr;


  /* Test sockslist_init() */
  test = sockslist_init(TEST_CAP);
  verify(test == 0, "sockslist_init() is sane");

  /* Test add and grow. */
  for(test = 1, i = 0; i < TEST_CAP; i++) {
    test = sockslist_add(dummy_socket, 20) > 0;
  }
  verify(test, "sockslist_add() returns successfully");

  for(test = 0, i = 0; i < TEST_CAP; i++) {
    test += sockslist_getfd(i);
  }
  verify(test = 20 * TEST_CAP, "sockslist_getfd() works and all sockets were in array");

  test = sockslist_add(dummy_socket, 20);
  verify(test, "sockslist_grow() doesn't kill everybody");
  
  test = sockslist_getfd(TEST_CAP);
  verify(test == 20, "sockslist_grow() works");

  /* Test remove */
  test = sockslist_remove(TEST_CAP);
  verify(test > 0, "sockslist_remove() is sane");

  sockslist_free();

  /* Test connection... */
  sockslist_init(1);

  test = sockslist_addConnect("google.com", "80");
  verify(test >= 0, "sockslist_addConnect() is sane");
  verify(sockslist_isConnected(test), "sockslist_addConnect() *thinks* it works.");

  test = sockslist_getfd(test);

  verify(send(test, "GET / HTTP/1.0\r\n\r\n", 18, MSG_DONTWAIT), "sockslist_addConnect() actually works");
  test = recv(test, buf, 13 * 1024, 0);
  verify(test > 0, "reply also works");

  freeaddrinfo(servinfo);
  sockslist_free();
  
  return 0;
}
