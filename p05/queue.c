#include <stdio.h>
#include "queue.h"


//------------------------------------------------------------------------------
// Insere um elemento no final da fila.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - o elemento deve existir
// - o elemento nao deve estar em outra fila

void queue_append(task_t **queue, task_t *elem) {
    
    int flag = 0;

    if (!queue){
        printf("\nQueueAppend_Error: Fila invalida.\n");        
        flag = 1;
    }else if(!elem){
        printf("\n QueueAppend_Error: Elemento invalido.\n");
        flag = 1;
    }else if(elem->next || elem->prev){
        printf("\n QueueAppend_Error: Elemento ja pertence a outra fila.\n");
        flag = 1;
    }    
    

    if(flag == 0) {

        if(queue_size (*queue) == 0){
            *queue = elem;
            elem->prev = elem;
            elem->next = elem;
        }else {
            task_t *aux = (*queue)->prev;
            
            aux->next = elem;
            
            elem->prev = aux;
            elem->next = (*queue);
            
            (*queue)->prev = elem;
        }

    }

}


//------------------------------------------------------------------------------
// Conta o numero de elementos na fila
// Retorno: numero de elementos na fila

int queue_size (task_t *queue) {

    if(!queue || !queue->next)
        return 0;

    task_t *aux;
    int count = 0;

    aux = queue;

    do{
        aux = aux->next;
        count++;
    }while(aux != queue);

    return count;
}

//------------------------------------------------------------------------------
// Remove o elemento indicado da fila, sem o destruir.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - a fila nao deve estar vazia
// - o elemento deve existir
// - o elemento deve pertencer a fila indicada
// Retorno: apontador para o elemento removido, ou NULL se erro

task_t *queue_remove (task_t **queue, task_t *elem){

    if (!queue){
        printf("\n QueueRemove_Error: Fila invalida.\n");        
        return NULL;
    }else if(queue_size <= 0){
        printf("\n QueueRemove_Error: Fila vazia.\n");
        return NULL;
    }else if(!elem){
        printf("\n QueueRemove_Error: Elemento invalido.\n");
        return NULL;
    }else if(!elem->next || !elem->prev){
        printf("\n QueueRemove_Error: Elemento não pertence a nenhuma fila.\n");
        return NULL;
    }  

    task_t *aux;

    aux = *queue;
    
    do{
        if(aux == elem){

            if(queue_size(*queue)==1){
                
                (*queue) = NULL;                
                elem->next = NULL;
                elem->prev = NULL;
                
                return elem;
            }

            if(elem == *queue){
                *queue = elem->next;
            }

            elem->next->prev = elem->prev;
            elem->prev->next = elem->next;
                       
            elem->next = NULL;
            elem->prev = NULL;
            return elem;
        }
        aux = aux->next;
    }while(aux != *queue);

    return NULL;  
};

//------------------------------------------------------------------------------
// Percorre a fila e imprime na tela seu conteúdo. A impressão de cada
// elemento é feita por uma função externa, definida pelo programa que
// usa a biblioteca. Essa função deve ter o seguinte protótipo:
//
// void print_elem (void *ptr) ; // ptr aponta para o elemento a imprimir

void queue_print (char *name, task_t *queue, void print_elem (void*) ){
    
    task_t *aux = queue;
    printf("%s[", name);

    if(queue_size(queue)>0) {
        do{
            printf(" ");
            print_elem(aux);
            aux = aux->next;
        }while(aux != queue);
    }
    printf("]\n");
};