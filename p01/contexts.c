#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// operating system check
#if defined(_WIN32) || (!defined(__unix__) && !defined(__unix) && (!defined(__APPLE__) || !defined(__MACH__)))
#warning Este codigo foi planejado para ambientes UNIX (LInux, *BSD, MacOS). A compilacao e execucao em outros ambientes e responsabilidade do usuario.
#endif

#define STACKSIZE 32768		/* tamanho de pilha das threads */
#define _XOPEN_SOURCE 600	/* para compilar no MacOS */

// declara as estruturas de contexto a serem utilizadas.
ucontext_t ContextPing, ContextPong, ContextMain;

/*****************************************************/

void BodyPing (void * arg)
{
   int i ;

   printf ("%s iniciada\n", (char *) arg) ;
   
   for (i=0; i<4; i++)
   {
      printf ("%s %d\n", (char *) arg, i) ;
      // troca o contexto de ContextPing para ContextPing
      swapcontext (&ContextPing, &ContextPong);
   }	
   printf ("%s FIM\n", (char *) arg) ;
    // troca com ContextMain
   swapcontext (&ContextPing, &ContextMain) ;
}

/*****************************************************/

void BodyPong (void * arg)
{
   int i ;

   printf ("%s iniciada\n", (char *) arg) ;

   for (i=0; i<4; i++)
   {
      printf ("%s %d\n", (char *) arg, i) ;
      // troca o contexto de ContextPing para ContextPing
      swapcontext (&ContextPong, &ContextPing);
   }
   printf ("%s FIM\n", (char *) arg) ;
   // troca com ContextMain
   swapcontext (&ContextPong, &ContextMain) ;
}

/*****************************************************/

int main (int argc, char *argv[])
{
   char *stack ;

   printf ("Main INICIO\n");

   getcontext (&ContextPing); // inicia a estrutura com os parametros da thread atual.

   stack = malloc (STACKSIZE) ; 
   if (stack)
   {
      ContextPing.uc_stack.ss_sp = stack ; // seta pilha de execução.
      ContextPing.uc_stack.ss_size = STACKSIZE; // tamanho da pilha de execução.
      ContextPing.uc_stack.ss_flags = 0; // 
      ContextPing.uc_link = 0; // define este contexto como principal.
   }
   else
   {
      perror ("Erro na cria��o da pilha: ");
      exit (1);
   }
    // modifica o contexto para executar a função BodyPing, atribui id=1 e seta o parametro para BodyPing.
   makecontext (&ContextPing, (void*)(*BodyPing), 1, "    Ping"); 

   getcontext (&ContextPong); // inicia a estrutura com os parametros da thread atual.

   stack = malloc (STACKSIZE) ;
   if (stack)
   {
      ContextPong.uc_stack.ss_sp = stack ;  // seta pilha de execução.
      ContextPong.uc_stack.ss_size = STACKSIZE; // tamanho da pilha de execução.
      ContextPong.uc_stack.ss_flags = 0; // flags (??)
      ContextPong.uc_link = 0; // define este contexto como principal.
   }
   else
   {
      perror ("Erro na cria��o da pilha: ");
      exit (1);
   }
   // modifica o contexto para executar a função BodyPong, atribui id=1 e seta o parametro para BodyPong.
   makecontext (&ContextPong, (void*)(*BodyPong), 1, "        Pong");
   
   // troca o contexto principal para ContextPing
   swapcontext (&ContextMain, &ContextPing);
   // troca o contexto principal para ContextPong
   swapcontext (&ContextMain, &ContextPong);

   printf ("Main FIM\n");

   exit (0);
}
