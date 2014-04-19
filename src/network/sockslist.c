#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "sockslist.h"

/* Boolean type. */
enum {FALSE, TRUE};

/**
 * struct SocketRecord
 * A logical connection between a socket file
 * descriptor and its information.
 **/
typedef struct SocketRecord {
  int socketfd;
  char isConnected;
  struct addrinfo* infoList;
  struct sockaddr* sockinfo;
} SocketRecord;

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
    
    if(freeNode->infoList) freeaddrinfo(freeNode->infoList);
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
  int initErr, sockfd, sockID;

  /* Clear out the hints structure. */
  memset(&hints, 0, sizeof hints);

  /* Be IP agnostic, start TCP stream. */
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((initErr = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(initErr));
    return -1;
  }

  /* Loop through all the results and connect to the first we can. */
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
			 p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }

    break;
  }

  /* Check client connection. */
  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return -2;
  }

  /* Good, add it to the socks list. */
  sockID = sockslist_add((struct sockaddr *)p->ai_addr, sockfd);
  /* Add the servinfo linked list. */
  records[sockID]->infoList = servinfo;
  records[sockID]->isConnected = TRUE;

  /* Return the Socket ID. */
  return sockID;
}

/**
 * int sockslist_getfd(int)
 * @return: Socket File Descriptor associated with record identified by @param id.
 **/
int sockslist_getfd(int id) {
  if(id >= size) return -1;
  return records[id]->socketfd;
}

/**
 * struct sockaddr* sockslist_getinfo(int)
 * @return: Socket information associated with record identified by @param id.                                                                                  
 **/
struct sockaddr* sockslist_getinfo(int id) {
  if(id >= size) return NULL;
  return records[id]->sockinfo;
}

/**
 * int sockslist_remove(int)
 * Disconnects from and closes socket connection.
 * @param id: the socket record to disconnect from and remove.
 * @return: id on success, negative on failure
 **/
int sockslist_remove(int id) {
  if (id >= size) return -1;
  
  close(records[id]->socketfd);
  records[id]->isConnected = FALSE;
  return id;
}
