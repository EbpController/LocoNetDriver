/*
 * file: ln.c
 * author: Geert Giebens, Jan van Hooydonk
 * comments: LocoNet driver
 * 
 */

#include "ln.h"

// <editor-fold defaultstate="collapsed" desc="initialisation">

#pragma interrupt_level 1       // the next routine is called once during
                                // initialisation (from main), the other
                                // calls are coming from the interrupt request
                                // so it is not necessary to duplicate this
                                // routine for reentrancy reasons
void lnInit(void)
{
    // declaration and initialisation (= malloc) of the RX and TX queue
    // essentially the queue is just a pointer to the instance of the struct
    lnTxQueue = malloc(sizeof(struct lnQueue*));
    lnInitQueue(lnTxQueue, 128u);    
    lnRxQueue = malloc(sizeof(struct lnQueue*));
    lnInitQueue(lnRxQueue, 128u);
    
    // initialisation of the other elements (comparator, EUSART, timer 1, ISR)
    lnInitComparator();
    lnInitEusart();
    lnInitTmr1();
    lnInitIsr();
    return;
}

void lnInitComparator(void)
{
    CMCON = 0b00000001;         // one indipendent comparator with output
                                // C1 Vin- = RA0
                                // C1 Vin+ = RA3
                                // C1 Vout = RA4
    TRISAbits.RA4 = false;      // PORTA 4 = comparator output
    return;
}

void lnInitEusart(void)
{
    // set pins for EUSART RX and TX
    TRISCbits.RC6 = false;      // PORTC 6 = LN TX
    TRISCbits.RC7 = true;       // PORTC 7 = LN RX
    
    // configure EUSART
    setBRG();
    BAUDCONbits.BRG16 = true;   // 16-bit baudrate generator
    BAUDCONbits.TXCKP = true;   // invert TX output signal
    TXSTAbits.SYNC = false;     // asynchronous mode
    TXSTAbits.BRGH = false;     // low speed
    TXSTAbits.TXEN = true;      // enable transmitter
    RCSTAbits.CREN = false;     // clear bit CREN to clear the OERR bit
    RCSTAbits.CREN = true;      // enable receiver
    _ = RCREG;                  // read the receive register to clear his
                                // content and to clear the FERR bit
    RCSTAbits.SPEN = true;      // enable serial port
    return;
}

void lnInitTmr1(void)
{
    TMR1H = 0x00;               // reset timer1
    TMR1L = 0x00;
    T1CON = 0b00110001;         // RD16 = 0 (timer1 in 8 bit operation)
                                // T1RUN = 0 (driven by another source)
                                // T1CKPS = 0b11 (1:8 prescaler)
                                // T1OSCEN = 0 (oscillator is disabled)
                                // T1SYNC = 0 (ignored)
                                // TMR1CS = 0 (source: internal clock = FOSC/4)
                                // TMR1ON = 1 (timer1 is enabled)
    return;
}

void lnInitIsr(void)
{
    IPR1bits.TMR1IP = 0;        // timer1 interrupt low priority
    IPR1bits.RCIP = 0;          // rxd interrupt low priority
    RCONbits.IPEN = 1;          // enable priority levels on iterrupt
    INTCONbits.GIEH = 1;        // enable all high priority interrupts
    INTCONbits.GIEL = 1;        // enable all low priority interrupts
    PIE1bits.RCIE = 1;          // enable rxd interrupt
    PIE1bits.TMR1IE = 1;        // enable timer1 overflow interrupt
    return;
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="ISR">

// <editor-fold defaultstate="collapsed" desc="ISR low priority">

// there are two possible low interrupt triggers, coming from
// the EUSART data receiver and/or coming from the timer 1 overrun flag
void __interrupt(low_priority) lnIsr(void)
{
    if (PIE1bits.TMR1IE && PIR1bits.TMR1IF)
    {
        // timer 1 interrupt (C + M + P detect)
        PIR1bits.TMR1IF = 0;        // clear timer 1 interrupt flag
        lnIsrTmr1();
    }
    else
    {
        if (PIE1bits.RCIE)
        {
            if (RCSTAbits.FERR)
            {
                // EUSART framing error (linebreak detected)
                _ = RCREG;          // read RCREG to clear the FERR bit
                if (lnRxCounter > 0)
                {
                    // clear LN RX counter and destroy LN RX message
                    lnRxCounter = 0;
                    free(lnRxMessage->lnData);
                    free(lnRxMessage);
                }
                if (lnTxCounter > 0)
                {
                    // clear LN TX counter and destroy LN TX message
                    lnTxCounter = 0;
                    free(lnTxMessage->lnData);
                    free(lnTxMessage);
                }
                if (LNCONbits.TMR1_MODE != 2)
                {
                    // if timer mode != 2 (timer in linebreak mode), meaning
                    // that the linebreak detection is not comming from this
                    // device, then restart the timer in linebrak mode
                    // this is necessary to extend the time in order to
                    // transmitting messages
                    startLinebreakExtension();
                }
            }
            else
            {
                // EUSART data received
                lnIsrRc();
            }
        }
    }
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="ISR Timer 1">

/**
 * interrupt routine for Timer 1
 */
void lnIsrTmr1(void)
{
    switch (LNCONbits.TMR1_MODE)
    {
    case 1:
        // after the linebreak (delay)
        RCSTAbits.SPEN = true;      // (re-)enable the receiver
        PORTCbits.RC6 = false;      // and restore output pin
        startCmpDelay();            // start the timer 1 with C + M + P delay
        break;
    case 2:
        // after the C + M + P delay
        //  if LN line is free: clear timer mode
        //  else: repeat C + M + P delay
        if (isLnFree())
        {
            LNCONbits.TMR1_MODE = 0;
        }
        else
        {
            startCmpDelay();        // restart the timer 1 with C + M + P delay
        }
        break;
    case 3:
        // after the restart and synchronisation of the BRG, start sending the
        // message
        txHandler();
        if (isLnFree())
        {
            LNCONbits.TMR1_MODE = 0;
            txHandler();
        }
        else
        {
            startCmpDelay();        // restart the timer 1 with C + M + P delay
        }
        break;
    default:
        break;
    }
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="ISR RX">

void lnIsrRc(void)
{
    uint8_t lnRxData = RCREG;            // get the last received byte
    
    if (lnTxCounter != 0)
    {
        // if device is in TX mode, check if received byte = transmitted byte
        if (lnTxData == lnRxData)
        {            
            // send next data of LN message
            txHandler();
        }
        else
        {
            // if LN RX data is not equal to LN TX data
            // then destroy LN TX message
            free(lnTxMessage->lnData);
            free(lnTxMessage);
            // and reset the LN TX counter
            lnTxCounter = 0;
            // send linebreak
            startLinebreak();
        }
    }
    else
    {
        // device is in RX mode (receive LN message)
        rxHandler(lnRxData);
    }
}

// </editor-fold>

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RX routines">

void rxHandler(uint8_t lnRxData)
{
    // start testing if msb = 1 (this is the startbyte of the LN message)
    if (lnRxCounter == 0)
    {
        if (lnRxData & 0b10000000u)
        {
            // lnRxmessage is a pointer to a struct of lnMessage_t
            // when first byte of message is readed, create a new pointer
            lnRxMessage = malloc(sizeof(struct lnMessage));

            // get the size of the message and put the first received byte
            // into LN message (5 lsb = OPCODE)
            lnRxMessage->size = (lnRxData >> 4) & 0b00000110u;
            lnRxMessage->opCode = lnRxData;
            lnRxMessage->checksum = lnRxData;
            lnRxCounter = 1;
        }
    }
    else
    {
        // depending on the size of the message, arrange the next
        // incomming data
        switch (lnRxMessage->size)
        {
        case 0:
            rxChecksum(lnRxQueue, lnRxMessage, lnRxData);
            break;
        case 6:
            if (lnRxCounter == 1)
            {
                lnRxMessage->size = lnRxData;
                lnRxMessage->lnData = malloc(sizeof(uint8_t) * lnRxMessage->size);
                lnRxCounter++;
            }
            else if (lnRxCounter <= lnRxMessage->size)
            {
                lnRxMessage->lnData[lnRxCounter - 2] = lnRxData;
                lnRxMessage->checksum ^= lnRxData;
                lnRxCounter++;
            }
            else
            {
                rxChecksum(lnRxQueue, lnRxMessage, lnRxData);
            }
            break;
        default:
            if (lnRxCounter <= lnRxMessage->size)
            {
                if (lnRxCounter == 1)
                {
                    lnRxMessage->lnData = malloc(sizeof(uint8_t) * lnRxMessage->size);
                }
                lnRxMessage->lnData[lnRxCounter - 1] = lnRxData;
                lnRxMessage->checksum ^= lnRxData;
                lnRxCounter++;
            }
            else
            {
                rxChecksum(lnRxQueue, lnRxMessage, lnRxData);
            }
            break;
        }
    }    
}

void rxChecksum(lnQueue_t* lnQueue, lnMessage_t* lnMessage, uint8_t checksum)
{
    if ((lnMessage->checksum + checksum) == 0xff)
    {
        // if checksum is correct than put message on queue
        enQueue(lnQueue, makeDeepCopy(lnMessage));
        // after receiving a message restart the CMP delay
        startCmpDelay();
    }
    else
    {
        // start a linebreak to let the transmitter known that the transmission
        // failed
        startLinebreak();
    }
    // this is the end of the message so destroy the used LN message
    free(lnMessage->lnData);
    free(lnMessage);
    // and reset the LN RX counter
    lnRxCounter = 0;
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="TX routines">

/**
 * routine for transmitting a byte over the LocoNet
 * @param the byte to transmit
 */
void sendLnMessage(void)
{
    // before start sending the message, test:
    //  - if timer1 is free (no CMP delay, no linebreak, no synchronisation)
    //  - if device is not in receiving or transitting mode (counters = 0)
    if ((LNCONbits.TMR1_MODE == 0) && (lnTxCounter == 0) && (lnRxCounter == 0))
    {
        if (!isQueueEmpty(lnTxQueue))
        {
            // get the oldest TX message from the TX queue (= head)
            lnTxMessage = makeDeepCopy(lnTxQueue->values[lnTxQueue->head]);
            // sync BRG before transmitting the first data byte
            syncBRG();
        }
    }
}

/**
 * routine that handles the transmission of the message
 */
void txHandler(void)
{
    if (isLnFree())
    {
        // the last transmited value (TXREG) must be stored (in lnTxData)
        // this is necessary to check if the data is transmitted correctly
        // (see routine rxHandler)
        if (lnTxCounter == 0)
        {
            TXREG = lnTxMessage->opCode;
            lnTxData = lnTxMessage->opCode;
            LNCONbits.TMR1_MODE = 0;
            lnTxCounter++;
        }
        else
        {
            if (lnTxCounter == lnTxMessage->size + 1)
            {
                // send checksum
                TXREG = lnTxMessage->checksum;
                lnTxData = lnTxMessage->checksum;
                deQueue(lnTxQueue);
            }
            else
            {
                TXREG = lnTxMessage->lnData[lnTxCounter - 1];
                lnTxData = lnTxMessage->lnData[lnTxCounter - 1];
                lnTxCounter++;
            }
        }    
    }
    else
    {
        // if line is not free
        // depending on the state of the TX counter:
        //  - start the CMP delay (if sending the message had not yet started)
        //  - start the linebreak (if sending the message had started)
        if (lnTxCounter == 0)
        {
            startCmpDelay();        // restart the timer 1 with C + M + P delay     
        }
        else
        {
            startLinebreak();
            lnTxCounter = 0;
        }
        // destroy LN TX message
        free(lnTxMessage->lnData);
        free(lnTxMessage);
    }
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="LN routines">

/**
 * check if the LocoNet is free (to use)
 * @return true: if the line is free, false: if the line is occupied
 */
bool isLnFree(void)
{
    // check if:
    //  RC7 = 1 (PORT C, bit 7 = high) ?
    //  RCIDL = 1 (receiver is idle = no data reception in progress) ?
    return (PORTCbits.RC7 && BAUDCONbits.RCIDL);
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Timer 1 routines">

/**
 * start the carrier + master + priority delay
 */
void startCmpDelay(void)
{
    // delay C + M + P = 1200탎 + 360탎 + random (between 0탎 and 1023탎)
    uint16_t delay = getRandomValue(lastRandomValue);
    lastRandomValue = delay;        // store last value of random generator
    delay &= 0x03ff;                // get random value between 0 and 1023
    delay += 1560u;                 // add C + M delay (= 1560탎)
    WRITETIMER1(~delay);            // set delay in timer 1
    
    LNCONbits.TMR1_MODE = 1;        // 1: timer 1 in C + M + P delay mode
}

/**
 * start the linebreak delay
 */
void startLinebreak(void)
{
    // linebreak detect by framing error
    RCSTAbits.SPEN = false;         // stop EUSART
    PORTCbits.RC6 = true;

    WRITETIMER1(~900);              // linebreak = 900탎
    LNCONbits.TMR1_MODE = 2;        // 2: timer 1 in linebreak mode
}

/**
 * start the linebreak delay (extension)
 */
void startLinebreakExtension(void)
{
    WRITETIMER1(~900);              // linebreak = 900탎
    LNCONbits.TMR1_MODE = 2;        // 2: timer 1 in linebreak mode
}

/**
 * random generator with Galois shift register
 * @param lfsr: initial value for the shift register
 * @return a 16 bit random vaule
 */
uint16_t getRandomValue(uint16_t lfsr)
{
    // with the help of a Galois LFSR (linear-feedback shift register)
    // we can get a random number. The LFSR is a shift register whose
    // input bit is a linear function of its previous state
    // refer: https://en.wikipedia.org/wiki/Linear-feedback_shift_register
    
    unsigned lsb = lfsr & 1u;  // get LSB (i.e. the output bit)
    lfsr >>= 1;                // shift register to right
    if (lsb)                   // if the output bit is 1
    {
        lfsr ^= 0xB400u;       //  apply toggle mask
    }
    return lfsr;
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="BRG settings">

/**
 * routine for synchronisation the BRG
 */
void syncBRG(void)
{
    // this routine handles the synchronisation of the BRG
    // it is important that the transmission of the message could be started
    // directly alter putting the data in the RCREG (the start of the
    // transmission depends on the state/value of the timer of the BRG)
    // a delay in the start of the transmission may lead to a none-detection
    // whether the line is still free
    // to make this possible restart the BRG and start a delay of
    // approximately 60탎
    setBRG();
    WRITETIMER1(~55);        // set delay approxity 60탎 (= 1 bit) in timer 1
    
    LNCONbits.TMR1_MODE = 3; // set timer 1 mode in synchronisation BRG
}

/**
 * sets and initialise the value of the bautrate generator
 */
void setBRG(void)
{
    SPBRGH = 0;                 // baudrate = 16.666
    SPBRG = 119u;               // ((32.000.000 / 16.666) / 16) - 1 = 119 (0x77)
}

// </editor-fold>
