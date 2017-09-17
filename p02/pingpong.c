#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>
#include "pingpong.h"	
#include "datatypes.h"

#define STACKSIZE 32768
#define DEBUG	

int global_id;
char *stack;

task_t system_task, *current_task;
ucontext_t system_context;

// Inicializa o sistema operacional; deve ser chamada no inicio do main()
void pingpong_init(){

    global_id = 1;
    system_task.tid = global_id;

    getcontext(&system_context);

    system_task.context = system_context;
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

    task->tid = global_id++;

    makecontext(&context, (void*) (*start_func), global_id, arg);
    task->context = context;

    #ifdef DEBUG
	printf("\ntask_create: criando tarefa %d", task->tid);
	#endif

    return (task->tid);

};

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exitCode) {
    
    #ifdef DEBUG
	printf("\ntask_exit: Terminando tarefa  %d.", current_task->tid);
	#endif

    task_switch(&system_task);
};

// alterna a execução para a tarefa indicada
int task_switch (task_t *task) {

    task_t *store_task = current_task;
    current_task = task;

    #ifdef DEBUG
    printf("\ntask_switch: task %d -> task %d.", store_task->tid, task->tid);
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