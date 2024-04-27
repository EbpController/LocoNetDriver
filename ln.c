/*
 * file: ln.c
 * author: Geert Giebens, Jan van Hooydonk
 * comments: LocoNet driver
 * 
 */

#include "ln.h"

// <editor-fold defaultstate="collapsed" desc="initialisation">

void lnInit(void)
{
    // declaration and initialisation (= malloc) of the RX and TX queue
    // essentially the queue is just a pointer to the instance of the struct
    initQueue(&lnTxQueue);
    initQueue(&lnTxTempQueue);
    initQueue(&lnRxQueue);
    initQueue(&lnRxTempQueue);
    
    // initialisation of the other elements (comparator, EUSART, timer 1, ISR)
    lnInitComparator();
    lnInitEusart();
    lnInitTmr1();
    lnInitIsr();
    lnInitLeds();
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
    T1CON = 0b00110000;         // RD16 = 0 (timer1 in 8 bit operation)
                                // T1RUN = 0 (driven by another source)
                                // T1CKPS = 0b11 (1:8 prescaler)
                                // T1OSCEN = 0 (oscillator is disabled)
                                // T1SYNC = 0 (ignored)
                                // TMR1CS = 0 (source: internal clock = FOSC/4)
                                // TMR1ON = 0 (timer1 is disabled)
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
    PIE1bits.TMR1IE = 1;        // enable timer 1 overflow interrupt

    T1CONbits.TMR1ON = 1;       // enable timer 1
    lastRandomValue = 1234u;    // set a random value != 0
    startCmpDelay();            // start LN driver with CMP delay
    return;
}

void lnInitLeds(void)
{
    TRISAbits.RA5 = 0;          // A5 as output
    LATAbits.LATA5 = 1;         // led 'data on LN' on (active low))
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
        // timer 1 interrupt
        // clear the interrupt flag and handle the request
        PIR1bits.TMR1IF = 0;
        lnIsrTmr1();
    }
    else if (PIE1bits.RCIE)
    {
        // EUSART RC interupt
        if (RCSTAbits.FERR)
        {
            // EUSART framing error (linebreak detected)
            // read RCREG to clear the FERR bit
            _ = RCREG;
            // retreive (recover) the last transmitted LN message
            recoverLnMessage(&lnTxTempQueue);
            // this framing error detection takes about 600탎
            // (10bits x 60탎) and a linebreak duration is specified at
            // 900탎, so add 300탎 after this detection time to complete
            // a full linebreak
            startLinebreak(300u);
        }
        else
        {
            // EUSART data received
            // handle the received data byte
            lnIsrRc();
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
        case 0:
            // LN driver is in idle mode
            if (isLnFree())
            {
                // LN is free
                if (!isQueueEmpty(&lnTxTempQueue))
                {
                    // if LN TX temporary queue is not empty restart
                    // the tramsmission (there is still something to be sent)
                    // this may occur when the last TX message was transmitted
                    // with errors (eg. after linebreak, conflict RX-TX, ...)
                    // start sync BRG before transmitting the first data byte
                    startSyncBRG();
                }
                else if (!isQueueEmpty(&lnTxQueue))
                {
                    // if LN TX queue has a LN message 
                    startTxLnMessage();
                }
                else
                {
                    // LN is free but nothing has to be transmitted
                    // restart timer 1 with idle delay
                    startIdleDelay();
                }
            }
            else
            {
                // LN is not free, so start timer 1 with CMP delay
                startCmpDelay();
            }
            break;
        case 1:
            // after the CMP delay
            if (isLnFree())
            {
                // if LN line is free start timer 1 with idle delay
                startIdleDelay();
            }
            else
            {
                // if LN line is not free restart timer 1 with CMP delay
                startCmpDelay();
            }
            break;
        case 2:
            // after the linebreak (delay) start CMP delay
            RCSTAbits.SPEN = true;      // (re-)enable the receiver
            PORTCbits.RC6 = false;      // and restore output pin
            startCmpDelay();            // start the timer 1 with CMP delay
            break;
        case 3:
            // after the synchronisation of the BRG start sending the LN message
            LNCONbits.TMR1_MODE = 0;
            txHandler();
            break;
        default:
            break;
    }
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="ISR RX">

void lnIsrRc(void)
{
    // get the received value
    uint8_t lnRxData = RCREG;

    if (!isQueueEmpty(&lnTxTempQueue))
    {
        // device is in TX mode
        // check if received byte = transmitted byte
        if (lnRxData == lnTxTempQueue.values[lnTxTempQueue.head])
        {
            // if last value is correct transmitted then dequeue
            deQueue(&lnTxTempQueue);
            if (!isQueueEmpty(&lnTxTempQueue))
            {
                // send next data of LN message untill queue is empty
                txHandler();
            }
            else
            {
                // restart CMP delay
                startCmpDelay();
            }
        }
        else
        {
            // if LN RX data is not equal to LN TX data send linebreak
            startLinebreak(900u);
        }
    }
    else
    {
        // device is in RX mode (receive LN message)
        rxHandler(lnRxData);
        // restart CMP delay
        startCmpDelay();
    }
}

// </editor-fold>

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RX routines">

void rxHandler(uint8_t lnRxData)
{
    // start testing if msb = 1 (this is the startbyte of the LN message)
    if ((lnRxData & 0x80) == 0x80)
    {
        clearQueue(&lnRxTempQueue);
        enQueue(&lnRxTempQueue, lnRxData);
    }
    else
    {
        enQueue(&lnRxTempQueue, lnRxData);

        // determine length of LN message
        uint8_t lnMessageLength;
        lnMessageLength = (lnRxTempQueue.values[lnRxTempQueue.head] & 0x60);
        lnMessageLength = (lnMessageLength >> 4) + 2;
        if (lnMessageLength > 6)
        {
            lnMessageLength = lnRxTempQueue.values[lnRxTempQueue.head + 1];
        }

        // has LN message reached the end the test checksum
        if (lnMessageLength == lnRxTempQueue.numEntries)
        {
            if (isChecksumCorrect(&lnRxTempQueue))
            {
                // if checksum is correct then copy LN RX temp queue to
                // LN RX queue
                while (!isQueueEmpty(&lnRxTempQueue))
                {
                    enQueue(&lnRxQueue, lnRxTempQueue.values[lnRxTempQueue.head]);
                    deQueue(&lnRxTempQueue);
                }
            }
        }
    }     
}

/**
 * calculate the checksum
 * @param lnQueue: name of the queue (pass the address of the queue)
 * @return true: if checksum is correct 
 */
bool isChecksumCorrect(volatile lnQueue_t* lnQueue)
{
    uint8_t checksum = 0;    
    for (uint8_t i = 0; i < lnQueue->numEntries; i++)
    {
        checksum ^= lnQueue->values[(lnQueue->head + i) % lnQueue->size];
    }    
    return (checksum == 0xff);
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="TX routines">

/**
 * start routine for transmitting a LN message
 * @param the byte to transmit
 */
void startTxLnMessage(void)
{
    // first, copy next LN message from LN TX queue into LN TX temporary queue
    do
    {
        enQueue(&lnTxTempQueue, lnTxQueue.values[lnTxQueue.head]);
        deQueue(&lnTxQueue);
    }
    while (!isQueueEmpty(&lnTxQueue) &&
            ((lnTxQueue.values[lnTxQueue.head] & 0x80) != 0x80));
    // sync BRG before transmitting the first data byte
    startSyncBRG();            
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
        TXREG = lnTxTempQueue.values[lnTxTempQueue.head];
    }
    else
    {
        // if line is not free start the linebreak
        startLinebreak(900u);
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
    //  RC7 = 1 (PORT C, bit 7 = high)
    //  RCIDL = 1 (receiver is idle = no data reception in progress)
    return (PORTCbits.RC7 && BAUDCONbits.RCIDL);
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Timer 1 routines">

void startIdleDelay(void)
{
    // delay = 1000탎
    WRITETIMER1(~1000);             // set delay in timer 1
    LNCONbits.TMR1_MODE = 0;        // 0: timer 0 in idle mode    
    // in idle mode, the led 'data on LN' can be turned off (active low)
    LATAbits.LATA5 = 1;
}

/**
 * start the carrier + master + priority delay
 */
void startCmpDelay(void)
{
    // delay CMP = 1200탎 + 360탎 + random (between 0탎 and 1023탎)
    uint16_t delay = getRandomValue(lastRandomValue);
    lastRandomValue = delay;        // store last value of random generator
    delay &= 1023u;                 // get random value between 0 and 1023
    delay += 1560u;                 // add C + M delay (= 1560탎)
    WRITETIMER1(~delay);            // set delay in timer 1
    LNCONbits.TMR1_MODE = 1;        // 1: timer 1 in CMP delay mode
    // led 'data on LN' on (active low)
    LATAbits.LATA5 = 0;
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
    
    uint16_t lsb = lfsr & 1u;  // get LSB (i.e. the output bit)
    lfsr >>= 1;                // shift register to right
    if (lsb)                   // if the output bit is 1
    {
        lfsr ^= 0xb400u;       //  apply toggle mask
    }
    return lfsr;
}

/**
 * start the linebreak delay (with a well defined time)
 * @param the time of the linebreak
 * @return 
 */
void startLinebreak(uint16_t time)
{
    // linebreak detect by framing error
    RCSTAbits.SPEN = false;         // stop EUSART
    PORTCbits.RC6 = true;
    // a LN linebreak definition 
    WRITETIMER1(~time);
    LNCONbits.TMR1_MODE = 2;        // 2: timer 1 in linebreak mode
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="BRG settings">

/**
 * routine to synchronize the BRG
 */
void startSyncBRG(void)
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
    WRITETIMER1(~60);        // set delay approxity 60탎 (= 1 bit) in timer 1
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
