#include "Scheduler.h"

#define     STARTUP       0b00000000
#define     READY         0b00000001
#define     RUNNING       0b00000010

//
//  Variables Locales privadas
//  Registros buffer para almacenar contexto
//
    
volatile reg8bit SchWREG=0;
volatile reg8bit SchBSR=0;
volatile reg8bit SchSTATUS=0;
volatile reg8bit SchPCL=0,SchPCH=0,SchPCU=0;
volatile reg8bit SchIntPCL=0,SchIntPCH=0,SchIntPCU=0;
volatile unsigned short long STCK_TEMP=0;
volatile reg8bit SchSTKPTR=0;
    
Task*   TaskQueue[MAX_TASK];
volatile reg8bit TaskCounter=0;
volatile reg8bit OS_STATUS;


// = = = = = = = = = = = = = = = = = =
//
//  Prototipos de funciones privadas
//
// = = = = = = = = = = = = = = = = = =
//Ejecutar o recuperar tarea actual
void TaskExecute(void);

void TaskRegister(Task* task,Task_stack stack,task_function function){
    if(TaskCounter<MAX_TASK){//Si es posible agregar la tarea
        Task_init(task,stack,function);//Inicializar
        TaskQueue[TaskCounter] = task;//Agregar
        TaskCounter++;
    }
}

/**
 * StartRTOS
 * 
 * Arranque del sistema, debe limpiar la pila y configurar los perifericos
 * necesarios para el control de procesos.
 * Hacer un llamado al Sistema y el Scheduler hace el resto.
 * 
 * @params  void
 * @return  void
 */
void StartRTOS(){
//
//  Configurar variables
//
    OS_STATUS   = 0;
    TaskCounter = MAX_TASK-1;
//
// Configurar perifericos
//
    //General interrupt enabled
    INTCONbits.GIE = 1;
    //Interrupt Priority Enabled
    INTCONbits.PEIE = 1;
    //T0 Interrupt Enable
    INTCONbits.TMR0IE = 1;
    //T0 Interrupt High Priority
    INTCON2bits.T0IP=1;
    //T0 Stop
    //T0 16 bits
    //T0 Internal Instruction Cycle Clock
    //Increment on rising edge
    //Prescaler not assigned
    //1:2 Prescaler
    T0CON=0b00001000;
    
    Scheduler();//Guardar direccion del Scheduler!
    
    //Limpiar Pila
    STKPTR = 0x00;
    TMR0 = 0xFFFE;
    T0CONbits.TMR0ON = 1;//Software Interrupt
    while(1);
}

void Scheduler(){
    //Registros imperdibles de estado
    //Deberian guardarse justo antes de esta ejecucion
    asm("PUSH");
    //Aqui se ejecutan las opciones del sistema operativo
    switch(OS_STATUS){
        case STARTUP://Arranque del OS
            //Guardar Direccion actual
            asm("MOVFF TOSL,_SchIntPCL");
            asm("MOVFF TOSH,_SchIntPCH");
            asm("MOVFF TOSU,_SchIntPCU");
            asm("POP");//Omitir direccion de esta funcion
            OS_STATUS = READY;
            return;//Volver al inicializador
        case READY://Ejecutar primer tarea
            asm("POP");
            //Seleccionar tarea
                TaskCounter++;
                TaskCounter %= MAX_TASK;
            //Cambiar estado del OS
                OS_STATUS <<= 1;
            //Ejecutar tarea
                TaskExecute();
            break;
        case RUNNING://Cambio de tarea
            //Guardar contexto actual
            TaskQueue[TaskCounter]->context_reg.BSR     = SchBSR;
            TaskQueue[TaskCounter]->context_reg.STATUS  = SchSTATUS;
            TaskQueue[TaskCounter]->context_reg.WREG    = SchWREG;
            TaskQueue[TaskCounter]->context_reg.STKPTR  = STKPTR;
        TaskQueue[TaskCounter]->context_reg.PC = TOS;
        asm("POP");
        //Guardar pila
        for(unsigned int i=TaskQueue[TaskCounter]->context_reg.STKPTR-1;i>0;i--){
            TaskQueue[TaskCounter]->context_reg.STACK[i] = TOS;
            asm("POP");
        }
        
            if(TaskQueue[TaskCounter]->State == TASK_RUNNING){
                TaskQueue[TaskCounter]->State = TASK_BLOCKED;
            }
            //Seleccionar tarea
                TaskCounter++;
                TaskCounter %= MAX_TASK;
            //Ejecutar tarea
                TaskExecute();
            break;
        default:
            Idle();
    }
    Idle();
}
    
void Idle(){
    while(1);
}

// = = = = = = = = = = = = = = = =
//
//  Manejo de interrupciones
//
// = = = = = = = = = = = = = = = =

void interrupt HISR(){
#asm
    MOVWF   _SchWREG            ;Guardar WREG
            
    MOVF    BSR,W
    MOVWF   _SchBSR             ;Guardar el registor BSR
    
    MOVF    STATUS,W
    MOVWF   _SchSTATUS  ;Guardar el registro STATUS
            
    MOVF    TOSU,W
    MOVWF   _SchPCU
    MOVF    TOSH,W
    MOVWF   _SchPCH
    MOVF    TOSL,W
    MOVWF   _SchPCL

    PUSH                ;Espacio nuevo en pila
#endasm
    if(TMR0IF){
        TMR0IF = 0;//Limpiar interrupcion
    }
//
// Volver al Scheduler
//
#asm
    MOVF    _SchIntPCU,W
    MOVWF   TOSU
    MOVF    _SchIntPCH,W
    MOVWF   TOSH
    MOVF    _SchIntPCL,W
    MOVWF   TOSL
#endasm
}

void interrupt low_priority LISR(){
    while(1);
}

// = = = = = = = = = = = = = = = = = =
//
//  Funciones locales y privadas
//
// = = = = = = = = = = = = = = = = = =

//Ejecutar o recuperar tarea actual
void TaskExecute(){
    STCK_TEMP=0;
    asm("POP");//Eliminar regreso al Scheduler
    if(TaskQueue[TaskCounter]->State == TASK_READY){
        TaskQueue[TaskCounter]->State = TASK_RUNNING;//Tarea corriendo
        TaskQueue[TaskCounter]->function();
        TaskQueue[TaskCounter]->State = TASK_DONE;//Tarea finalizada
    }
    if(TaskQueue[TaskCounter]->State == TASK_BLOCKED){
        //Recuperar contexto y ejecutar
        SchWREG   = TaskQueue[TaskCounter]->context_reg.WREG;
        SchBSR    = TaskQueue[TaskCounter]->context_reg.BSR;
        SchSTATUS = TaskQueue[TaskCounter]->context_reg.STATUS;
        
        for(unsigned char i=0;i<TaskQueue[TaskCounter]->context_reg.STKPTR;i++){
            STCK_TEMP = TaskQueue[TaskCounter]->context_reg.STACK[i];
            SchPCU  = STCK_TEMP>>16;
            SchPCH  = STCK_TEMP>>8;
            SchPCL  = STCK_TEMP;
            asm("MOVF  _SchPCU,W");
            asm("MOVWF TOSU");
            asm("MOVF  _SchPCH,W");
            asm("MOVWF TOSH");
            asm("MOVF  _SchPCL,W");
            asm("MOVWF TOSL");
            asm("PUSH");
        }
            STCK_TEMP = TaskQueue[TaskCounter]->context_reg.PC;
            SchPCU  = STCK_TEMP>>16;
            SchPCH  = STCK_TEMP>>8;
            SchPCL  = STCK_TEMP;
            asm("MOVF  _SchPCU,W");
            asm("MOVWF TOSU");
            asm("MOVF  _SchPCH,W");
            asm("MOVWF TOSH");
            asm("MOVF  _SchPCL,W");
            asm("MOVWF TOSL");

        
        asm("MOVF  _SchSTATUS,W");
        asm("MOVWF STATUS");
        asm("MOVF  _SchBSR,W");
        asm("MOVWF BSR");
        asm("MOVF  _SchWREG,W");
        return;//Regresar a la tarea
    }
    while(1);
}