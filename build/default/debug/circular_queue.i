# 1 "circular_queue.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:/Users/user/.mchp_packs/Microchip/PIC18Fxxxx_DFP/1.4.151/xc8\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "circular_queue.c" 2







# 1 "./circular_queue.h" 1
# 13 "./circular_queue.h"
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdbool.h" 1 3
# 13 "./circular_queue.h" 2

# 1 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdint.h" 1 3



# 1 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\musl_xc8.h" 1 3
# 5 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdint.h" 2 3
# 26 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdint.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\bits/alltypes.h" 1 3
# 133 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned __int24 uintptr_t;
# 148 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\bits/alltypes.h" 3
typedef __int24 intptr_t;
# 164 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\bits/alltypes.h" 3
typedef signed char int8_t;




typedef short int16_t;




typedef __int24 int24_t;




typedef long int32_t;





typedef long long int64_t;
# 194 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long long intmax_t;





typedef unsigned char uint8_t;




typedef unsigned short uint16_t;




typedef __uint24 uint24_t;




typedef unsigned long uint32_t;





typedef unsigned long long uint64_t;
# 235 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned long long uintmax_t;
# 27 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdint.h" 2 3

typedef int8_t int_fast8_t;

typedef int64_t int_fast64_t;


typedef int8_t int_least8_t;
typedef int16_t int_least16_t;

typedef int24_t int_least24_t;
typedef int24_t int_fast24_t;

typedef int32_t int_least32_t;

typedef int64_t int_least64_t;


typedef uint8_t uint_fast8_t;

typedef uint64_t uint_fast64_t;


typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;

typedef uint24_t uint_least24_t;
typedef uint24_t uint_fast24_t;

typedef uint32_t uint_least32_t;

typedef uint64_t uint_least64_t;
# 148 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdint.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\bits/stdint.h" 1 3
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
# 149 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdint.h" 2 3
# 14 "./circular_queue.h" 2

# 1 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdlib.h" 1 3
# 10 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdlib.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\features.h" 1 3
# 11 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdlib.h" 2 3
# 21 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdlib.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\bits/alltypes.h" 1 3
# 24 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long int wchar_t;
# 128 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned size_t;
# 22 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdlib.h" 2 3

int atoi (const char *);
long atol (const char *);

long long atoll (const char *);

double atof (const char *);


float strtof (const char *restrict, char **restrict);
double strtod (const char *restrict, char **restrict);
long double strtold (const char *restrict, char **restrict);



long strtol (const char *restrict, char **restrict, int);
unsigned long strtoul (const char *restrict, char **restrict, int);

long long strtoll (const char *restrict, char **restrict, int);
unsigned long long strtoull (const char *restrict, char **restrict, int);


unsigned long __strtoxl(const char * s, char ** endptr, int base, char is_signed);

unsigned long long __strtoxll(const char * s, char ** endptr, int base, char is_signed);
# 55 "C:\\Program Files\\Microchip\\xc8\\v2.45\\pic\\include\\c99\\stdlib.h" 3
int rand (void);
void srand (unsigned);

void *malloc (size_t);
void *calloc (size_t, size_t);
void *realloc (void *, size_t);
void free (void *);

          void abort (void);
int atexit (void (*) (void));
          void exit (int);
          void _Exit (int);

void *bsearch (const void *, const void *, size_t, size_t, int (*)(const void *, const void *));







__attribute__((nonreentrant)) void qsort (void *, size_t, size_t, int (*)(const void *, const void *));

int abs (int);
long labs (long);

long long llabs (long long);


typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;

typedef struct { long long quot, rem; } lldiv_t;


div_t div (int, int);
ldiv_t ldiv (long, long);

lldiv_t lldiv (long long, long long);


typedef struct { unsigned int quot, rem; } udiv_t;
typedef struct { unsigned long quot, rem; } uldiv_t;
udiv_t udiv (unsigned int, unsigned int);
uldiv_t uldiv (unsigned long, unsigned long);
# 15 "./circular_queue.h" 2


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
_Bool isQueueEmpty(lnQueue_t*);
_Bool isQueueFull(lnQueue_t*);
_Bool enQueue(lnQueue_t*, lnMessage_t*);
_Bool deQueue(lnQueue_t*);
lnMessage_t* makeDeepCopy(lnMessage_t*);
# 8 "circular_queue.c" 2







void lnInitQueue(lnQueue_t* q, uint8_t size)
{
    q->size = size;
    q->values = malloc(sizeof(struct lnMessage*) * q->size);
    q->numEntries = 0;
    q->head = 0;
    q->tail = 0;
}






_Bool isQueueEmpty(lnQueue_t* q)
{
    return (q->numEntries == 0);
}





   _Bool isQueueFull(lnQueue_t* q)
{
    return (q->numEntries == q->size);
}







_Bool enQueue(lnQueue_t* q, lnMessage_t* value)
{

    if (isQueueFull(q))
    {

       return 0;
    }
    else
    {

        q->values[q->tail] = value;
        q->numEntries++;
        q->tail = (q->tail + 1) % q->size;
        return 1;
    }
}






_Bool deQueue(lnQueue_t* q)
{

    if (isQueueEmpty(q))
    {

        return 0;
    }
    else
    {

        q->head = (q->head + 1) % q->size;
        q->numEntries--;
        return 1;
    }
}






lnMessage_t* makeDeepCopy(lnMessage_t* lnMessageOrig)
{


    lnMessage_t* lnMessageCopy;
    lnMessageCopy = malloc(sizeof(struct lnMessage));


    lnMessageCopy->opCode = lnMessageOrig->opCode;
    lnMessageCopy->checksum = lnMessageOrig->checksum;
    lnMessageCopy->size = lnMessageOrig->size;




    lnMessageCopy->lnData = malloc(sizeof(uint8_t) * lnMessageCopy->size);
    for (uint8_t i = 0; i < lnMessageCopy->size; i++)
    {
        lnMessageCopy->lnData[i] = lnMessageOrig->lnData[i];
    }


    return lnMessageCopy;
}
