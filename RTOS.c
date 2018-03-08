#include "RTOS.h"

void init_tasks(){
    //  Guardar el contexto del main u origen de ejecución
    rtos.main_task.wreg     = WREG;
    rtos.main_task.status   = STATUS;
    rtos.main_task.bsr      = BSR;
    rtos.main_task.pcu      = TOSU;
    rtos.main_task.pch      = TOSH;
    rtos.main_task.pcl      = TOSL;
    //Guardar el contexto del administrador de tareas
    asm("PUSH");
    if(rtos.first_exec){
        rtos.manager_PC_U = TOSU;
        rtos.manager_PC_H = TOSH;
        rtos.manager_PC_L = TOSL-2;
        //
        //  Con esto retornaremos hasta el push
        //  y evitaremos que pop underborde la pila
        //
        TMR0=0;
        T0CONbits.TMR0ON = 1;//Activamos el timer
        rtos.first_exec = false;
    }
    asm("POP");
    asm("CLRWDT");
    //
    //Primer lanzamiento de tareas
    //
THREAD_TEST:
    if(launcher_mode){
        rtos.task_counter++;
        if(rtos.task_counter <= MAX_THREADS){
                actual_thread = &rtos.task[ rtos.task_counter-1 ];
                task_father(&rtos.task[ rtos.task_counter -1 ]);
                asm("GOTO $");
        }else{
            launcher_mode = false;
            rtos.task_counter = 0;
            actual_thread = &rtos.main_task;
            task_recovery(&rtos.main_task);
            asm("GOTO $");
        }
    }else{
        rtos.task_counter++;
        if(rtos.task_counter <= MAX_THREADS){
            actual_thread = &rtos.task[ rtos.task_counter-1 ];
            if(rtos.task[ rtos.task_counter-1 ].life){
                //Si la tarea tiene vida se ejecuta
                task_recovery(&rtos.task[ rtos.task_counter-1 ]);
                asm("GOTO $");
            }else{
                //Sino se prueba con la siguiente
                TMR0=0;
                goto THREAD_TEST;
            }
        }else{
            rtos.task_counter=0;
            if(!rtos.task[0].life &&\
                        !rtos.task[1].life &&\
                        !rtos.task[2].life &&\
                        !rtos.task[3].life){
                    goto ALL_DEATH;
            }else{
                    actual_thread = &rtos.main_task;
                    task_recovery(&rtos.main_task);
                    asm("GOTO $");
            }
        }
    }
ALL_DEATH:
    T0CONbits.TMR0ON = 0;
    //
    //  Recuperar contexto del main
    //
    task_recovery(&rtos.main_task);
}
void interrupt HISR(){
    if(*hisr){
        hisr();
    }
    if(TMR0IF){
        //
        //Vaciar pila excepto los primeros 2 elementos
        //
            actual_thread->wreg     = WREG;
            actual_thread->bsr      = BSR;
            actual_thread->status   = STATUS;
            actual_thread->pcl      = TOSL;
            actual_thread->pch      = TOSH;
            actual_thread->pcu      = TOSU;
            //
            //  AGREGAR A LA PILA
            //
            
            //
            //  
            //
        global_pcu    = rtos.manager_PC_U;
        global_pch    = rtos.manager_PC_H;
        global_pcl    = rtos.manager_PC_L;
        
        TMR0IF = 0;
        
        STATUS = global_status;
        BSR = global_bsr;
        TOSU = global_pcu;
        asm("MOVF _global_pcu,W");
        asm("MOVWF TOSU");
        asm("MOVF _global_pch,W");
        asm("MOVWF TOSH");
        asm("MOVF  _global_pcl,W");
        asm("MOVWF TOSL");
//        asm("RETFIE");
    }
}
void interrupt low_priority LISR(){
    if(lisr){
        lisr();
    }
}
void task_father(thread* task){
    if(task->function){
        task->life = true;
        task->function();
        task->life = false;
    }
}
void task_recovery(thread* task){
        //
        //Recuperar pila
        //
        global_status = task->status;
        global_bsr    = task->bsr;
        global_wreg   = task->wreg;
        global_pcl    = task->pcl;
        global_pch    = task->pch;
        global_pcu    = task->pcu;
//        STATUS = global_status;
        asm("MOVF _global_status,W");
        asm("MOVWF  STATUS");
//        BSR = global_bsr;
        asm("MOVF _global_bsr,W");
        asm("MOVWF  BSR");
//        TOSU = global_pcu;
        asm("MOVF _global_pcu,W");
        asm("MOVWF TOSU");
        asm("MOVF _global_pch,W");
        asm("MOVWF TOSH");
        asm("MOVF  _global_pcl,W");
        asm("MOVWF TOSL");
        asm("MOVF  _global_wreg,W");
        asm("RETURN");
}
void set_task(void (*function)(),unsigned char task){
    switch(task){
        case 0:
            rtos.task[0].function = function;
            break;
        case 1:
            rtos.task[1].function = function;
            break;
        case 2:
            rtos.task[2].function = function;
            break;
        case 3:
            rtos.task[3].function = function;
            break;
        default:
            return;
    }
}