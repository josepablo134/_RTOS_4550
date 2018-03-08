//#include "pic_configuration.h"
//#include "RTOS.h"
//#define     DONE    {while(1){}}
//unsigned char __delay_counter1_;
//unsigned char __delay_counter2_;
//#define  simple_delay(){for(__delay_counter1_=0;\
//                            __delay_counter1_<255;\
//                            __delay_counter1_++){\
//                                for(__delay_counter2_=0;\
//                                __delay_counter2_<255;\
//                                __delay_counter2_++){}\
//                            }}
//
//void interrupt_hp();
//void toggleA0();
//void toggleA4();
//void task_0();
//void task_1();

#include "RTOS/RTOS4550.h"

Task_stack_element  T0StackStruct[31];
Task                Task0_Handle;
Task_stack_element  T1StackStruct[31];
Task                Task1_Handle;

volatile unsigned var0=0;
volatile unsigned var1=0;


void Task0(void);
void Task1(void);

void main(void) {
    TaskRegister(&Task0_Handle,T0StackStruct,Task0);
    TaskRegister(&Task1_Handle,T1StackStruct,Task1);
    //Comenzar a ejecutar las tareas!
    StartRTOS();
    while(1);//Never reached!
}

//
//  Al parecer no se pueden crear variables
//      locales en las tareas
//
void Task0(){
    while(1){
        var0++;
    }
}

void Task1(){
    while(1){
        var1++;
    }
}

//void interrupt_hp(){
//    return;
//}
//void task_0(){
//    PORTA = 0;
//    LATA = 0;
//    TRISA = 0;
//    ADCON1 = 0x0F;
//    CMCON = 0x07;
//    volatile unsigned char a=0x20; 
//    while(1){
//        LATAbits.LA4 = 1;
//        simple_delay();
//        LATAbits.LA4 = 0;
//        simple_delay();
//    }
//}
//void task_1(){
//    PORTA = 0;
//    LATA = 0;
//    TRISA = 0;
//    ADCON1 = 0x0F;
//    CMCON = 0x07;
//    while(1){
//        simple_delay();
//        LATAbits.LA0 = 1;
//        simple_delay();
//        LATAbits.LA0 = 0;
//    }
//}