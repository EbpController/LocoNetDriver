/* 
 * file: circular_queue.c
 * author: Geert Giebens, Jan van Hooydonk
 * comments: LocoNet driver
 *
 */

#include "circular_queue.h"

/**
 * initialisation of the queue
 * @param q: name of the queue (pass the address of the queue)
 * @param size: size of the queue
 */
void lnInitQueue(lnQueue_t* q, uint8_t size)
{
    q->size = size;
    q->values = malloc(sizeof(struct lnMessage*) * q->size);
    q->numEntries = 0;  // queue is empty
    q->head = 0;
    q->tail = 0;
}

/**
 * check if the queue is empty
 * @param q: name of the queue (pass the address of the queue)
 * @return true: if queue is empty, false; if queue is not empty
 */
bool isQueueEmpty(lnQueue_t* q)
{
    return (q->numEntries == 0);
}

/**
 * check if the queue is full
 * @param q: name of the queue (pass the address of the queue)
 * @return true: if queue is full, false; if queue is not full
 */bool isQueueFull(lnQueue_t* q)
{
    return (q->numEntries == q->size);
}

/**
 * put a value on the queue
 * @param q: name of the queue (pass the address of the queue)
 * @param value: the value to put on the queue
 * @return true: if value is put on the queue, false; if queue is full
 */
bool enQueue(lnQueue_t* q, lnMessage_t* value)
{
    // put a value on the queue
    if (isQueueFull(q))
    {
       // return false if queue is full
       return false;
    }
    else
    {
        // put the value to the queue and return true
        q->values[q->tail] = value;
        q->numEntries++;
        q->tail = (q->tail + 1) % q->size;
        return true;
    }
}

/**
 * get a value from the queue
 * @param q: name of the queue (pass the address of the queue)
 * @return true: if last value is get from the queue, false; if queue is empty
 */
bool deQueue(lnQueue_t* q)
{
    // get a value from the queue
    if (isQueueEmpty(q))
    {
        // return false if queue is empty
        return false;
    }
    else
    {
        // set the values and return true
        q->head = (q->head + 1) % q->size;
        q->numEntries--;
        return true;
    }
}

/**
 * make a deep copy of the LN message
 * @param lnMessageOrig: the original LN message
 * @return the cloned LN message
 */
lnMessage_t* makeDeepCopy(lnMessage_t* lnMessageOrig)
{
    // declaration and initialisation of new instance of a pointer to the
    // struct of lnMessage
    lnMessage_t* lnMessageCopy;
    lnMessageCopy = malloc(sizeof(struct lnMessage));
    
    // copy value variables
    lnMessageCopy->opCode = lnMessageOrig->opCode;
    lnMessageCopy->checksum = lnMessageOrig->checksum;
    lnMessageCopy->size = lnMessageOrig->size;
    
    // copy pointer variables
    // first, make new instance of a pointer to an array of uint8_t
    // then, copy values
    lnMessageCopy->lnData = malloc(sizeof(uint8_t) * lnMessageCopy->size);
    for (uint8_t i = 0; i < lnMessageCopy->size; i++)
    {
        lnMessageCopy->lnData[i] = lnMessageOrig->lnData[i];
    }
    
    // return the cloned LN message
    return lnMessageCopy;
}
