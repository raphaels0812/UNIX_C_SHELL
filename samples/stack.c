#include <stdio.h>
#include <stdlib.h>
#define STACK_SIZE 10

typedef struct Node{
    int value;
    struct Node *next;
}node;

node *initStack();
int isEmpty(node *stack);
void push(node *stack, int value);
node *pop(node *stack);
void nodeCheckMallocOrRealloc(node *node);

node *initStack(){
    node *stack = (node *)malloc(sizeof(node));
    nodeCheckMallocOrRealloc(stack);
    stack->value = 0;
    stack->next = NULL;
    return stack;
}

int isEmpty(node *stack){
    if(stack->next == NULL)
        return 1;
    else
        return 0;
}

void push(node *stack, int value){
    node *new = (node *)malloc(sizeof(node));
    nodeCheckMallocOrRealloc(new);
    new->value = value;
    new->next = NULL;

    if(isEmpty(stack)){
        stack->next = new;
    }
    else{
        node *temp = stack->next;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = new;
    }
}

node *pop(node *stack){
    if(isEmpty(stack) == 1){
        return NULL;
    }
    else{
        node *last = stack->next;
        node *nextToLast = stack->next;
        while(last->next != NULL){
            nextToLast = last;
            last = last->next;
        }
        nextToLast->next = last;
        return last;
    }
}

void nodeCheckMallocOrRealloc(node *node){
    if(node == NULL){
        printf("allocation error\n");
        exit(EXIT_FAILURE);
    }
}