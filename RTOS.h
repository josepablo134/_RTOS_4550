#ifndef RTOS_H
#define RTOS_H
    #include "pic_configuration.h"
    #define MAX_THREAD          4
    #define DefaultTimerData    35000

    #define READY               0b00000001
    #define BLOCKED             0b00000010
    #define RUNNING             0b00000100
    #define DONE                0b00001000

    void interrupt HISR();
    void interrupt low_priority LISR();
    
    
    typedef struct{
            unsigned short long     *Stack;
            unsigned char           StackSize;
            unsigned char           StackPtr;
            unsigned char           status;
            unsigned char           bsr;
            unsigned char           wreg;
            unsigned short long     pc;
            unsigned char           state;
            void (*function)();
    }thread;
    
    typedef unsigned short long StackStruct;
    typedef unsigned char  StackSize;
    typedef void (*thread_function)(void);
    
    void (*hisr)() = 0x00;
    void (*lisr)() = 0x00;
    
    void Scheduler(void);
    void Task_register(thread*);
    
    void Task_init(thread*, thread_function , StackStruct* , StackSize);
    
    void setup_rtos();

    #define reload_timer(){TMR0 = DefaultTimerData;}
    
    #define SoftwareInterrupt(){TMR0=35530;}
    #define set_high_interrupt_vector(HISR){isr = HISR;}
    #define set_low_interrupt_vector(LISR) {lsr = LISR;}
#endif