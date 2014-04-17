#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>

#include "sockslist.h"

/**
 * struct SocketRecord
 * A logical connection between a socket file
 * descriptor and its information.
 **/
static struct SocketRecord {
  int socketfd;
  char isConnected;
  struct addrinfo* infoList;
  struct sockaddr* sockinfo;
};

/* Master socket record vector. */
static SocketRecord** records;
static int capacity, size;

static int sockslist_grow() {
  SocketRecord** newRecords;
  capacity *= 2;

  newRecords = realloc(records, sizeof(SocketRecord*) * capacity);

  if(newRecords == NULL) {
    perror("sockslist_grow: realloc");
    return -1;
  }

  records = newRecords;
  return EXIT_SUCCESS;
}

/**
 * int sockslist_init(int)
 * Initializes socket records.
 * @return: 0 on success, negative on failure.
 **/
int sockslist_init(int initialCapacity) {
  if(initialCapacity < 1) {
    fprintf(stderr, "sockslist_init: Invalid Initial Capacity.");
    return -1;
  }

  records = (struct SocketRecord**)malloc(initialCapacity * sizeof(SocketRecord*));

  if(!records) {
    perror("sockslist_init: malloc");
    return -1;
  }

  capacity = initialCapacity;
  size = 0;

  return 0;
}

/**
 * void sockslist_free(void)
 * Disconnects from and frees all socket records.
 **/
void sockslist_free(void) {
  SocketRecord* freeNode;
  int i;
  for(i = 0; i < size; i++) {
    freeNode = records[i];
    
    if(freeNode->infoList) freeaddrinfo(infoList);
    else free(freeNode->sockinfo);
    
    if(freeNode->isConnected) close(freeNode->socketfd);
    free(freeNode);
  }
  free(records);
}

/**
 * int addSocket(struct sockaddr*, int)
 * Adds a new socket to the sockslist, does not try to connect.
 * @param socketfd: Socket File Descriptor.
 * @param socketInfo: Socket information generated from getaddrinfo() or accept()
 * @return: The ID associated with this socket record, or negative on failure.
 **/
int sockslist_add(struct sockaddr* socketinfo, int socketfd) {
  SocketRecord* newRecord;
  assert(socketinfo != NULL);

  newRecord = malloc(sizeof(SocketRecord));

  if(newRecord == NULL) {
    perror("sockslist_add: malloc");
    return -1;
  }

  if(size >= capacity) {
    if(sockslist_grow()) return -1;
  }

  records[size] = newRecord;
  size++;
  return size-1;
}


/**
 * int sockslist_addConnect(char*, int)
 * Creates and attempts to connect to a new socket.
 * @param port: string representation of target port number or service.
 * @param host: string representation of domainname or ip address (v4 or v6)
 * @return: the ID associated with this socket record, or negative on failure.
 **/
int sockslist_addConnect(char* host, char* port) {
  struct addrinfo hints, *servinfo, *p;
  int initErr;
