/* 
 * file: circular_queue.h
 * author: Geert Giebens, Jan van Hooydonk
 * comments: LocoNet driver
 *
 * revision history:
 *  v1.0 Creation (14/01/2024 15:28)
 */

#ifndef CIRCULAR_QUEUE_H
#define	CIRCULAR_QUEUE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct lnMessage
{
    uint8_t size;
    uint8_t opCode;
    uint8_t checksum;
    uint8_t* lnData;
} lnMessage_t;

typedef struct lnQueue
{
    uint8_t head, tail, numEntries, size;           
    lnMessage_t** values;
} lnQueue_t;

void lnInitQueue(lnQueue_t*, uint8_t);
bool isQueueEmpty(lnQueue_t*);
bool isQueueFull(lnQueue_t*);
bool enQueue(lnQueue_t*, lnMessage_t*);
bool deQueue(lnQueue_t*);
lnMessage_t* makeDeepCopy(lnMessage_t*);

#endif	/* CIRCULAR_QUEUE_H */

