#include "RTOS.h"

void init_tasks(){
    asm("PUSH");
    if(task_counter==0){
        //Guardar el estado de esta función
        myBSR       = BSR;
        mySTATUS    = STATUS;
        myWREG      = WREG;
        myPC        = TOS;
        T0CONbits.TMR0ON = 1;
    }
    asm("POP");
    while(1){
        task_counter++;
        switch(task_counter){
            case 1:
                task0.function();
                break;
            case 2:
                task1.function();
                break;
            default:
                task_counter=3;
                break;
        }
    }
}
void interrupt HISR(){
    if(hisr){
        hisr();
    }
    if(TMR0IF){
        // Limpiar la interrupción
        TMR0IF = 0;
        // Limpiar el wdt
        asm("CLRWDT");
        // Regresar al controlador de tareas
            //Omitimos el valor cargado en la pila
            asm("POP");
        WREG    = myWREG;
        STATUS  = mySTATUS;
        BSR     = myBSR;
        TOS     = myPC;
    }
}

void interrupt low_priority LISR(){
    if(lisr){
        lisr();
    }
}