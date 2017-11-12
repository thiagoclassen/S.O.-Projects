// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional

#ifndef __DATATYPES__
#define __DATATYPES__

#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>

// Estrutura que define uma tarefa
typedef struct task_t {

  int tid;
  struct task_t *next, *prev;
  ucontext_t context;
  int dynamic_priority;
  int static_priority;
  int level;
  struct itimerval timer;
  unsigned int startTime;
  unsigned int activeTime;
  unsigned int calls;

} task_t;

// estrutura que define um semáforo
typedef struct
{
  // preencher quando necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
  // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{
  // preencher quando necessário
} mqueue_t ;

#endif
