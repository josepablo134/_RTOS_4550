#include "RTOS.h"
#define     MAXSTACKSIZE    31
#define     EXEC_DONE    {while(1){}}

#define  simple_delay(){for(unsigned char i=0;i<255;i++){}}


StackStruct    Task0Stack[MAXSTACKSIZE];
StackStruct    Task1Stack[MAXSTACKSIZE];
thread task0;
thread task1;

void task_0();
void task_1();

void main(void) {
    setup_rtos();
    
    Task_init(&task0,task_0,Task0Stack,MAXSTACKSIZE);
    Task_init(&task1,task_1,Task1Stack,MAXSTACKSIZE);
    
    Task_register(&task0);
    Task_register(&task1);

    Scheduler();
    EXEC_DONE;
}

void task_0(){
    PORTA = 0;
    LATA = 0;
    TRISA = 0;
    ADCON1 = 0x0F;
    CMCON = 0x07;
    while(1){
        LATAbits.LA4 = 1;
        simple_delay();
        LATAbits.LA4 = 0;
        simple_delay();
    }
}

void task_1(){
    PORTA = 0;
    LATA = 0;
    TRISA = 0;
    ADCON1 = 0x0F;
    CMCON = 0x07;
    while(1){
        LATAbits.LA0 = 1;
        simple_delay();
        LATAbits.LA0 = 0;
        simple_delay();
    }
}