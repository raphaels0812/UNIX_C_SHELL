#include <stdlib.h>
#include <stdio.h>

typedef struct Elem{
    char *info;
    struct Elem *next;
}elem;

elem *initQueue(){
    elem *queue = (elem *)malloc(sizeof(elem));
    queue->info = NULL;
    queue->next = NULL;
}

int isEmptyQueue(elem *queue){
    if(queue->next == NULL)
        return 1;
    else
        return 0;
}

void insertElem(elem *queue, char *info){
    elem *new = (elem *)malloc(sizeof(elem));
    new->info = info;
    new->next = NULL;
    if(isEmptyQueue(queue) == 1){
        queue->next = new;
    }
    else{
        elem *aux = queue;
        while(aux->next != NULL){
            aux = aux->next;
        }
        aux->next = new;
    }
}

char *removeElem(elem *queue){
    char *info;
    if(isEmptyQueue(queue) == 1){
        return NULL;
    }
    else{
        elem *head = queue;
        elem *aux = head->next;
        head->next = aux->next;
        info = aux->info;
        return info;
    }
}

void printQueue(elem *queue){
    int k =0;
    elem *aux = queue;
    while(aux->next != NULL){
        aux = aux->next;
        printf("queue[%d] = %s\n", k, aux->info);
        k++;
    }
}