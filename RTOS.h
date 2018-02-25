#ifndef RTOS_H
#define RTOS_H
    #include <xc.h>
    #include <pic18f4550.h>

    void interrupt HISR();
    void interrupt low_priority LISR();
    
    volatile unsigned char task_counter = 0;
    volatile unsigned char       myBSR;
    volatile unsigned char       myWREG;
    volatile unsigned char       mySTATUS;
    volatile unsigned short long myPC;
    
    
    typedef struct{
            unsigned status;
            unsigned bsr;
            unsigned wreg;
            unsigned short long pc;
            void (*function)();
    }thread;
    
    volatile thread task0;
    volatile thread task1;
    
    void (*hisr)() = 0x00;
    void (*lisr)() = 0x00;
    
    void init_tasks(void);
    
    void rtos_delay(unsigned short);
    
    #define setup_rtos() {INTCONbits.GIE = INTCONbits.PEIE = 1;\
                         INTCONbits.TMR0IE = INTCON2bits.T0IP=1;\
                         T0CON=0b00000000;TMR0 = 35530;}

    #define set_high_interrupt_vector(HISR){isr = HISR;}
    #define set_low_interrupt_vector(LISR) {lsr = LISR;}
#endif