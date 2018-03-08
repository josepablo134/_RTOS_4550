#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
    #include "task.h"
    #include "pic_configuration.h"
    
    #define MAX_TASK    2

    typedef unsigned char reg8bit;
    
    //Registrar tarea en el Scheduler!
    void TaskRegister(Task*,Task_stack,task_function);
    void StartRTOS(void);
    
    void Scheduler(void);
    void Idle(void);
    
    void interrupt HISR(void);
    void interrupt low_priority LISR(void);
#endif