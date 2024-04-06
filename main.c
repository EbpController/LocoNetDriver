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
    
    
    TRISBbits.TRISB1 = 0; // A0 as output
    
    lnInit();
    
    while (1)        
    {
        LATBbits.LATB1 = 0;
        __delay_ms(500);
        LATBbits.LATB1 = 1;
        __delay_ms(500);
    }
    return;
    
}
