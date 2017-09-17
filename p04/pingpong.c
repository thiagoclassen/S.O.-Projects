#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>
#include "pingpong.h"	
#include "datatypes.h"
#include "queue.h"

#define STACKSIZE 32768
#define DEBUG	

int global_id;
char *stack;
task_t *userTasks;

task_t system_task, *current_task, dispatcher_task;
ucontext_t system_context, dispatcher_context;

void aging(task_t *next_task){
    task_t *aux;
    
    aux = userTasks;

    do{
        //printf("\nTestando se: %d != %d. \n", aux->tid, next_task->tid);
        if(aux->tid != next_task->tid){
          //  printf("\nDiferente troca\n");
            aux->dynamic_priority--;            
        } else {
            //printf("\nIgual n troca\n");
        }
        aux = aux->next;
    }while(aux != userTasks);
}

task_t* selectTask(){
    task_t *aux, *next;
    
    aux = userTasks;
    next = userTasks;

    do{
        //printf("\nTestando se: %d > %d. \n", next->priority, aux->priority);
        if(next->dynamic_priority > aux->dynamic_priority){
            //printf("\nPrioridade anterior %d, atual %d...efetuando troca. \n", 
                //next->priority, aux->priority);
            next = aux;            
        }
        aux = aux->next;
    }while(aux != userTasks);

    next->dynamic_priority = next->static_priority;

    return next;
}

task_t* scheduler(){
    return selectTask();
    //return userTasks;
};

void dispatcher_body () {

    task_t *next;

    while ( queue_size(userTasks) > 0 ) {
        next = scheduler(); // scheduler é uma função
        if (next) {
            aging(next);
            task_switch (next);
        }
    }
    task_exit(0) ; 
}

int create_dispatcher (task_t *task, void (*start_func)(void *), void *arg) {

    stack = malloc(STACKSIZE);

    if(!stack)    
        return -1;

    ucontext_t context;

    getcontext(&context);
    
    context.uc_stack.ss_sp = stack ;
    context.uc_stack.ss_size = STACKSIZE;
    context.uc_stack.ss_flags = 0;
    context.uc_link = 0;

    task->tid =++ global_id;

    makecontext(&context, (void*) (*start_func), global_id, arg);
    task->context = context;

    #ifdef DEBUG
	printf("\ntask_create: criando dispatcher com tid: %d.\n", task->tid);
	#endif

    return (task->tid);

};

// Inicializa o sistema operacional; deve ser chamada no inicio do main()
void pingpong_init(){

    global_id = 1;
    system_task.tid = global_id;

    getcontext(&system_context);

    system_task.context = system_context;

    #ifdef DEBUG
	printf("\nsystem_init: criando system context com tid: %d.\n", system_task.tid);
	#endif

    create_dispatcher(&dispatcher_task, dispatcher_body, "");

    current_task = &system_task;

    /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf (stdout, 0, _IONBF, 0) ;
};

// Cria uma nova tarefa. Retorna um ID> 0 ou erro.
// task_t *task --> descritor da nova tarefa
// void (*start_func)(void *) --> funcao corpo da tarefa
// void *arg --> argumentos para a tarefa
int task_create (task_t *task, void (*start_func)(void *), void *arg) {

    stack = malloc(STACKSIZE);

    if(!stack)    
        return -1;

    ucontext_t context;

    getcontext(&context);
    
    context.uc_stack.ss_sp = stack ;
    context.uc_stack.ss_size = STACKSIZE;
    context.uc_stack.ss_flags = 0;
    context.uc_link = 0;

    task->tid =++ global_id;

    makecontext(&context, (void*) (*start_func), global_id, arg);
    task->context = context;
    task->static_priority = 0;
    task->dynamic_priority = task->static_priority;
    queue_append (&userTasks, task);

    #ifdef DEBUG
	printf("\ntask_create: criando tarefa %d\n", task->tid);
	#endif

    return (task->tid);

};

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exitCode) {

    #ifdef DEBUG
	//printf("\ntask_exit: Terminando tarefa  %d.\n", current_task->tid);
	#endif
    
    queue_remove(&userTasks, current_task);
    
    /*if(current_task->tid > 3){
        free(current_task->context.uc_stack.ss_sp);
    }*/

    if(queue_size(userTasks) != 0){
        task_switch(&dispatcher_task);
    } else {
        task_switch(&system_task);
    }


};

// alterna a execução para a tarefa indicada
int task_switch (task_t *task) {

    task_t *store_task = current_task;
    current_task = task;

    #ifdef DEBUG
    //printf("\ntask_switch: task %d -> task %d.", store_task->tid, task->tid);
    #endif 

    swapcontext(&store_task->context, &current_task->context);

    return -1;
};

// retorna o identificador da tarefa corrente (main eh 0)
int task_id () {
    
    #ifdef DEBUG
    printf("\ntask_id: %d.", current_task->tid);
    #endif

    return current_task->tid;
};

// suspende uma tarefa, retirando-a de sua fila atual, adicionando-a à fila
// queue e mudando seu estado para "suspensa"; usa a tarefa atual se task==NULL
void task_suspend (task_t *task, task_t **queue) {

};

// acorda uma tarefa, retirando-a de sua fila atual, adicionando-a à fila de
// tarefas prontas ("ready queue") e mudando seu estado para "pronta"
void task_resume (task_t *task) {

};

// libera o processador para a próxima tarefa, retornando à fila de tarefas
// prontas ("ready queue")
void task_yield () {
    //if(!current_task->next)
        task_switch(&dispatcher_task);
    //task_switch(current_task->next);
};

// define a prioridade estática de uma tarefa (ou a tarefa atual)
void task_setprio (task_t *task, int prio) {
    task->static_priority = prio;
    task->dynamic_priority = task->static_priority;
};

// retorna a prioridade estática de uma tarefa (ou a tarefa atual)
int task_getprio (task_t *task) {
    return current_task->static_priority;
};