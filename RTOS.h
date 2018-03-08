#ifndef RTOS_H
#define RTOS_H
    #define MAX_THREADS         3
    #define MAX_VARS            16
    #define MAX_STACK_LEVEL     5
    #include <xc.h>
    #include <pic18f4550.h>
    #include <stdbool.h>
    void interrupt HISR();
    void interrupt low_priority LISR();
    
    volatile unsigned char global_status;
    volatile unsigned char global_bsr;
    volatile unsigned char global_wreg;
    volatile unsigned char global_pcl;
    volatile unsigned char global_pch;
    volatile unsigned char global_pcu;
    
    typedef struct{
            unsigned char REG[MAX_VARS];
            unsigned short long STACK[MAX_STACK_LEVEL];
            unsigned char STCKPTR;
    }software_stack;
    
    typedef struct{
            unsigned char status;
            unsigned char bsr;
            unsigned char wreg;
            unsigned char pcl;
            unsigned char pch;
            unsigned char pcu;
            bool          life;
            software_stack stack;
            void (*function)();
    }thread;
    typedef struct{
        bool            first_exec;
        unsigned char   task_counter;
        unsigned char   manager_PC_U;
        unsigned char   manager_PC_H;
        unsigned char   manager_PC_L;
        thread          task[MAX_THREADS];
        thread          main_task;
    }system_buffer;
    
    volatile system_buffer rtos;
    volatile bool launcher_mode;
    volatile thread* actual_thread = 0x00;
    
    void (*hisr)() = 0x00;
    void (*lisr)() = 0x00;
    /**
     * Agrega un elemento a la pila de la tarea
     * @param   unsigned short long dato que se desea agregar
     * @param   thread  tarea que se desea modificar
     * @return  true si fue exitoso, false sino
     */
    bool push(unsigned short long,thread*);
    /**
     * Hacer pop a la pila de la tarea
     * @param thread  tarea que se desea modificar
     * @return 
     */
    unsigned short long pop(thread*);
    
    void init_tasks(void);
    /**
     * Esta es la tarea padre, quien detecta si ha terminado o no la ejecución
     * 
     */
    void task_father(thread*);
    /**
     * Permite restaurar el contexto de la tarea
     */
    void task_recovery(thread*);
    /**
     * Permite asignar una tarea a la lista de tareas
     * @param function
     * @param task
     */
    void set_task(void (*function)(),unsigned char task);
    /**
     * Permite generar un delay dentro de las tareas
     * @param 
     */
    void rtos_delay(unsigned short);
    /**
     * Inicializamos los parametros del RTOS
     * Ningun task, ningun main_task, task_counter reseted!
     * primera ejecución aún no dada
     */
    #define setup_rtos() {INTCONbits.GIE = 1;INTCONbits.PEIE = 1;\
                         INTCONbits.TMR0IE = 1;INTCON2bits.T0IP=1;\
                         T0CON=0b00000000;TMR0 = 35530;\
                         launcher_mode = true;\
                         rtos.first_exec = true;\
                         rtos.task_counter=0;\
                         rtos.main_task.function = 0x00;\
                         for(unsigned char i=0;i<MAX_THREADS;i++){\
                         rtos.task[i].function = 0x00;\
                         rtos.task[i].life = false;\
                         rtos.task[i].stack.STCKPTR = 0x00;}\
                         rtos.main_task.life = false;\
                         rtos.main_task.stack.STCKPTR = 0x00;}
    /**
     * Mapear, redireccionar el vector de interrupciones de alta prioridad
     */
    #define set_high_interrupt_vector(HISR){hisr = HISR;}
    /**
     * Mapear, redireccionar el vector de interrupciones de alta prioridad
     */
    #define set_low_interrupt_vector(LISR) {lisr = LISR;}
#endif