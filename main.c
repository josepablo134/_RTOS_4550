#include "pic_configuration.h"
#include "RTOS.h"
#define     DONE    {while(1){}}

#define  simple_delay(){for(unsigned char i=0;i<255;i++){}}

void task_0();
void task_1();

void main(void) {
    setup_rtos();
    task0.function = task_0;
    task1.function = task_1;
    
    init_tasks();
    DONE;
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