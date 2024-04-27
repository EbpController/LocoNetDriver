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
#include "config.h"
#include "circular_queue.h"

void lnInit(void);
void lnInitComparator(void);
void lnInitEusart(void);
void lnInitTmr1(void);
void lnInitIsr(void);
void lnInitLeds(void);

void lnIsr(void);
void lnIsrTmr1(void);
void lnIsrRcError(void);
void lnIsrRc(void);

void rxHandler(uint8_t);

void startTxLnMessage(void);
void txHandler(void);
bool isChecksumCorrect(volatile lnQueue_t*);

bool isLnFree(void);

void startIdleDelay(void);
void startCmpDelay(void);
void startLinebreak(uint16_t);
void startSyncBRG(void);
void setBRG(void);

uint16_t getRandomValue(uint16_t);

// LN flag register
typedef struct
    {
        unsigned TMR1_MODE :2;      // 0 = idle
                                    // 1 = running CMP delay
                                    // 2 = running linebreak
                                    // 3 = running synchronisation BRG
    } LNCONbits_t;
LNCONbits_t LNCONbits;

// LN used varibles
uint8_t _;                          // dummy variable
uint16_t lastRandomValue;  // initial value for the random generator

volatile lnQueue_t lnTxQueue;
volatile lnQueue_t lnTxTempQueue;
volatile lnQueue_t lnRxQueue;
volatile lnQueue_t lnRxTempQueue;

#endif	/* LN_H */

