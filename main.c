#include "pic_configuration.h"
#include "RTOS.h"
#define     DONE    {while(1){}}
unsigned char __delay_counter1_;
unsigned char __delay_counter2_;
#define  simple_delay(){for(__delay_counter1_=0;\
                            __delay_counter1_<255;\
                            __delay_counter1_++){\
                                for(__delay_counter2_=0;\
                                __delay_counter2_<255;\
                                __delay_counter2_++){}\
                            }}

void interrupt_hp();
void toggleA0();
void toggleA4();
void task_0();
void task_1();

void main(void) {
    setup_rtos();
    //set_high_interrupt_vector(interrupt_hp);
    set_task(task_0,0);
    set_task(task_1,1);
    init_tasks();
    DONE;
}

void interrupt_hp(){
    return;
}
void task_0(){
    PORTA = 0;
    LATA = 0;
    TRISA = 0;
    ADCON1 = 0x0F;
    CMCON = 0x07;
    volatile unsigned char a=0x20; 
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
        simple_delay();
        LATAbits.LA0 = 1;
        simple_delay();
        LATAbits.LA0 = 0;
    }
}