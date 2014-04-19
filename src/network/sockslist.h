/**
 * File: sockslist.h
 * Author: Ethan Gordon
 *
 * AO Vector of Sockets used by the network module.
 **/

#ifndef DATAQUEUE_H
#define DATAQUEUE_H

#include <sys/types.h>
#include <sys/socket.h>

/**
 * int sockslist_init(int)
 * Initializes socket records.
 * @return: 0 on success, negative on failure.
 **/
int sockslist_init(int initialCapacity);

/**
 * void sockslist_free(void)
 * Disconnects from and frees all socket records immediately.
 **/
void sockslist_free(void);

/**
 * int sockslist_add(struct sockaddr*, int)
 * Adds a new socket to the sockslist, does not try to connect.
 * @param socketfd: Socket File Descriptor.
 * @param socketInfo: Socket information generated from getaddrinfo() or accept()
 * @return: The ID associated with this socket record, or negative on failure.
 **/
int sockslist_add(struct sockaddr* socketInfo, int socketfd);

/**
 * int sockslist_addConnect(char*, int)
 * Creates and attempts to connect to a new socket.
 * @param port: string representation of target port number or service.
 * @param host: string representation of domainname or ip address (v4 or v6)
 * @return: the ID associated with this socket record, or negative on failure.
 **/
int sockslist_addConnect(char* host, char* port);

/**
 * int sockslist_getfd(int)
 * @return: Socket File Descriptor associated with record identified by @param id.
 **/
int sockslist_getfd(int id);

/**
 * struct sockaddr* sockslist_getinfo(int)
 * @return: Socket information associated with record identified by @param id.
 **/
struct sockaddr* sockslist_getinfo(int id);

/**
 * int sockslist_remove(int)
 * Disconnects from and closes socket connection.
 * @param id: the socket record to disconnect from and remove.
 * @return: id on failure, negative on success
 **/
int sockslist_remove(int id);

/**
 * char sockslist_isConnected(int)
 * @return Whether record is a connected socket or not.
 **/
char sockslist_isConnected(int id);

#endif
