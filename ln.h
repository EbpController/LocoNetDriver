/* 
 * file: ln.h
 * author: Geert Giebens, Jan van Hooydonk
 * comments: LocoNet driver
 *
 * revision history:
 *  v1.0 Creation (14/01/2024 15:28)
 */

// this is a guard condition so that contents of this file are not included
// more than once
#ifndef LN_H
#define	LN_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "config.h"
#include "circular_queue.h"

void lnInit(void);
void lnInitComparator(void);
void lnInitEusart(void);
void lnInitTmr1(void);
void lnInitIsr(void);

void lnIsr(void);
void lnIsrRcError(void);
void lnIsrRc(void);
void rxHandler(uint8_t);
void rxChecksum(lnQueue_t*, lnMessage_t*, uint8_t);
void lnIsrTmr1(void);

void syncBRG(void);
void setBRG(void);
void sendLnMessage(void);
void txHandler(void);
void startCmpDelay(void);
void startLinebreak(void);
void startLinebreakExtension(void);

uint16_t getRandomValue(uint16_t);
bool isLnFree(void);

// LN flag register
typedef struct
    {
        unsigned TMR1_MODE :2;      // 0 = free
                                    // 1 = running C + M + P delay
                                    // 2 = running linebreak
                                    // 3 = running synchronisation BRG
    } LNCONbits_t;
LNCONbits_t LNCONbits;

// LN used varibles
uint8_t _;                          // dummy variable
uint8_t lnRxCounter;  
uint8_t lnTxCounter; 
uint8_t lnTxData;
uint16_t lastRandomValue;           // initial value for the random generator

lnMessage_t* lnRxMessage;
lnMessage_t* lnTxMessage;
lnQueue_t* lnTxQueue;
lnQueue_t* lnRxQueue;

#endif	/* LN_H */

