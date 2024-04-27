/*
 * file: main.c
 * author: Geert Giebens, Jan van Hooydonk
 * comments: LocoNet driver
 *
 * revision history:
 *  v1.0 Creation (14/01/2024 15:28)
 */

#include "config.h"
#include "ln.h"

void main(void)
{
    // startup
    
    // set oscillator to 32MHz
    OSCTUNE = 0x00;
    OSCCON = 0x70;
    
    while (OSCCONbits.IOFS == 0)
    {
        NOP();
    }    
    OSCTUNEbits.PLLEN = 1;    
    
    // init LN    
    lnInit();
    
    TRISBbits.TRISB1 = 0; // A0 as output
    while (1)        
    {
        LATBbits.LATB1 = 1;
        __delay_ms(50);
        LATBbits.LATB1 = 0;
        __delay_ms(50);
        
        di();
        enQueue(&lnTxQueue, 0xb2);
        enQueue(&lnTxQueue, 0x00);
        enQueue(&lnTxQueue, 0x00);
        enQueue(&lnTxQueue, 0x4d);
        ei();
    }
    return;
    
}
