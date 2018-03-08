#ifndef _TASK_H_
#define _TASK_H_
    
    #define TASK_READY      0b00000001
    #define TASK_RUNNING    0b00000010
    #define TASK_BLOCKED    0b00000100
    #define TASK_DONE       0b00000000
    
    typedef void(*task_function)(void);
    typedef unsigned short long  Task_stack_element;
    typedef unsigned short long* Task_stack;
    
    typedef struct Context{
        unsigned char WREG;
        unsigned char BSR;
        unsigned char STATUS;
        unsigned short long PC;
        unsigned short long *STACK;
        unsigned char STKPTR;
    }Context;
    
    typedef struct Task{
        Context         context_reg;
        unsigned char   State;
        task_function   function;
    }Task;
    
    void Task_init(Task*,Task_stack,task_function);

#endif