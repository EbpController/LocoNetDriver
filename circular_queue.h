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

#define QUEUE_SIZE 128

typedef struct lnQueue_t
{
    uint8_t head;
    uint8_t tail;
    uint8_t numEntries;
    uint8_t size;
    uint8_t values[QUEUE_SIZE];
} lnQueue_t;

void initQueue(volatile lnQueue_t*);
bool isQueueEmpty(volatile lnQueue_t*);
bool isQueueFull(volatile lnQueue_t*);
bool enQueue(volatile lnQueue_t*, uint8_t);
bool deQueue(volatile lnQueue_t*);
void clearQueue(volatile lnQueue_t*);
void recoverLnMessage(volatile lnQueue_t*);

#endif	/* CIRCULAR_QUEUE_H */

