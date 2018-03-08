#include "task.h"

void Task_init(Task* task,Task_stack stack,task_function function){
    task->context_reg.WREG         = 0x00;
    task->context_reg.BSR          = 0x00;
    task->context_reg.STATUS       = 0x00;
    task->context_reg.PC           = 0x00;
    task->context_reg.STKPTR       = 0x00;
    task->context_reg.STACK        = stack;
    
    task->function                  = function;
    task->State                     = TASK_READY;
}