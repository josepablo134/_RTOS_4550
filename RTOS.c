#include "RTOS.h"

volatile unsigned char      Scheduler_flag=0;
volatile unsigned char      TaskCounter=0;
volatile unsigned char       SchBSR=0;
volatile unsigned char       SchWREG=0;
volatile unsigned char       SchSTATUS=0;
volatile unsigned char long SchPC=0;
volatile unsigned char      SchPCL=0;
volatile unsigned char      SchPCH=0;
volatile unsigned char      SchPCU=0;

//StackStruct RTOS_STACK[31];
thread*  Scheduler_Tasks[MAX_THREAD];//Arreglo de apuntadores a thread

void setup_rtos() {
    INTCONbits.GIE = INTCONbits.PEIE = 1;
    INTCONbits.TMR0IE = INTCON2bits.T0IP=1;
    T0CON=0b00000000;TMR0 = DefaultTimerData;
//    for(unsigned char i=0;i<31;i++){
//        RTOS_STACK[i] = 0x00;//Limpiar pila de sistema
//    }
}

void Task_execute(thread* Task){
    asm("POP");//Ignorar contexto anterior!
    if(Task->state & READY){
        Task->function();
    }
    Task->state = DONE;//Tarea finalizada!
    SoftwareInterrupt();//Cambiar de tarea!
    while(1);
}

void Scheduler(){
    asm("PUSH");//Aqui comienza el llamado al Scheduler desde la interrupcion
    if(!Scheduler_flag){
        //Guardar el estado de esta función
        SchPC        = TOS;
        
        TaskCounter     = 0;
        Scheduler_flag  = 1;//Evitar repetir inicializacion
    }
    asm("POP");//Ignorar Contexto anterior!
SCHEDULER_LOOP:
//El sistema debe estar listo para conmutar entre funciones
//La pila debe estar completamente vacia

    //Preparar siguiente ejecucion
    T0CONbits.TMR0ON = 1;//Comenzar tiempo de tarea!
    if(Scheduler_Tasks[TaskCounter]->state & READY){
        //No ha sido ejecutada
        Task_execute(Scheduler_Tasks[TaskCounter]);
    }else if(Scheduler_Tasks[TaskCounter]->state & BLOCKED){
        //Continuar ejecucion
            //Cargar pila de la tarea!
            for(unsigned i=0;i<Scheduler_Tasks[TaskCounter]->StackPtr;i++){
                TOS     = Scheduler_Tasks[TaskCounter]->Stack[i];
                asm("PUSH");
            }
            TOS = Scheduler_Tasks[TaskCounter]->pc;//Cargar retorno!
            
            //Cargar el contexto de la tarea
            STATUS  = Scheduler_Tasks[TaskCounter]->status;
            BSR     = Scheduler_Tasks[TaskCounter]->bsr;
            WREG    = Scheduler_Tasks[TaskCounter]->wreg;
            //Regresar a la ejecucion
            return;
    }//Cualquier opcion omite la tarea
    TaskCounter++;
    TaskCounter%=MAX_THREAD;
    goto SCHEDULER_LOOP;
}

void Task_init(thread* Task, void (*thread_function)(void) , StackStruct* SStruct, StackSize SSize){
    Task->bsr       = 0x00;
    Task->pc        = 0x00;
    Task->state     = READY;//Tarea lista para ser ejecutada
    Task->status    = 0x00;
    Task->wreg      = 0x00;
    Task->function  = thread_function;
    Task->Stack     = SStruct;
    Task->StackSize = SSize;
    Task->StackPtr  = 0;//StackPtr
}

void Task_register(thread* Task){
    if(TaskCounter<MAX_THREAD){
        Scheduler_Tasks[TaskCounter] = Task;
        TaskCounter++;
    }
}

void interrupt HISR(){
//La interrupcion de alta prioridad se ejecuta cuando haya
//habido una interrupcion y se esten ejecutando tareas
//Por lo tanto, al llegar aqui se llega desde el contexto
//de una tarea
    SchWREG     = WREG;//Capturar WREG y registros de contexto
    SchBSR      = BSR;
    SchSTATUS   = STATUS;
//Apagar el timer
    T0CONbits.TMR0ON = 0;
    // Limpiar el wdt
    asm("CLRWDT");
//Recuperar contexto en la tarea actual
    unsigned char   STKPTR_buffer=0;
    STKPTR_buffer = STKPTR;
    Scheduler_Tasks[TaskCounter]->StackPtr = STKPTR_buffer;
    while(STKPTR_buffer>1){//Mientras hayan elementos en la pila
        Scheduler_Tasks[TaskCounter]->Stack[STKPTR_buffer] = TOS;//POP desde pila!
        asm("POP");
        STKPTR_buffer = STKPTR;
    }
    //Copiar registros de contexto!
    Scheduler_Tasks[TaskCounter]->bsr   = SchBSR;
    Scheduler_Tasks[TaskCounter]->wreg  = SchWREG;
    Scheduler_Tasks[TaskCounter]->status= SchSTATUS;
    
    //Ejecutar Interrupt Service Runtime
    if(hisr){
        hisr();
    }
//De tratarse de una interrupcion de OS
    if(TMR0IF){
        // Limpiar la interrupción
        TMR0IF = 0;
        // Regresar al controlador de tareas
            //Omitimos el valor cargado en la pila
        #asm
            MOVFF ,TOSL
            MOVFF ,TOSH
            MOVFF ,TOSU
        #endasm
        return;
    }else{
//De haber sido la ejecucion de una interrupcion
//durante una tarea restaurar contexto y regresar a la tarea
        STKPTR = Scheduler_Tasks[TaskCounter]->StackPtr;
        // Limpiar el wdt
        asm("CLRWDT");
        T0CONbits.TMR0ON = 1;//Continuar tiempo de ejecucion
        return;
    }
}

void interrupt low_priority LISR(){
    T0CONbits.TMR0ON = 0;//Apagar el timer
    if(lisr){
        lisr();
    }
}