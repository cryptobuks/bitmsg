/**
 * File: dataQueue.h
 * Author: Ethan Gordon
 *
 * Data Queue: Two Abstract Queue Objects
 * One lines up objects to be sent.
 * The other holds received objects from the network.
 **/

struct bmPacket {
  /* Index in the socket list, can be used to retrieve node information. */
  int socket_index;
  int len_payload;
  char* payload;
}


/**
 * int queue_init(void)
 * Initializes the sendQueue and the sendQueue pthread.
 * @return: 0 on success, -1 on failure (and sets errno)
 **/
int queue_init(void);

/**
 * int queue_send(struct bmPacket*)
 * Enqueues @param bmPacket to be sent out to the network.
 * Does NOT block. NOTE: bmPacket will be free'd upon being sent.
 * @return: 0 on success, -1 on failure (and sets errno)
 **/ 
int queue_send(struct bmPacket*);

/**
 * struct bmPacket* queue_recv(void)
 * Retrieves the most recenty received packet from the network.
 * This BLOCKS if the queue is empty. NOTE: bmPacket must be freed by client.
 * @param timeout: number of seconds to block. Blocks indefinitely if 0.
 * @return bmPacket: Most recent received packet from network, NULL on failure
 **/
struct bmPacket* queue_recv(time_t timeout);

/**
 * void queue_free(void)
 * Empties and frees all memory in the queue immediately.
 * All messages left in the queue are NOT sent.
 **/
void queue_free(void);
