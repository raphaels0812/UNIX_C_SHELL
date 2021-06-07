#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "commands.c"
#include "stack.c"
#include "queue.c"

void clearBuffer(char **buffer, int buffSize);
void bufferCheckMallocOrRealloc(char **buffer);
void cmdCheckMallocOrRealloc(commandLine *cmd);
int executeCommand(commandLine *_command, elem *queue);

int main(int argc, char **argv){
    /*built-in para saída do programa*/
    if(strcmp(argv[0], "exit") == 0){
        return 0;
    } 

    /*uso incorreto ("ls" eh uma excecao)*/
    if(argc == 1 && strcmp(argv[0], "ls") != 0){             
        printf("Correct use: %s <command> <arg1> <arg2> ... <argn>\n", argv[0]);
        return 0;
    }

    /*buffer para armazenar cada comando simples e seus argumentos*/
    int maxBuffSize = 10;
    char **buffer = (char **)malloc(maxBuffSize*sizeof(char**));
    bufferCheckMallocOrRealloc(buffer);
    int currBufferIndex = 1;

    /*struct para armazenas linha de comandos*/
    commandLine *cmd;
    cmd = (commandLine *)malloc(sizeof(commandLine*));
    cmdCheckMallocOrRealloc(cmd);
    initCommandLine(cmd);
    
    /*fila para armazenar os operadores especiais*/
    elem *queue = initQueue();
    int queueSize = 0;        
    
    /*itera sobre os itens da matriz de argumentos (argv)*/
    for(int i = 1; i < argc; i++){
        
        //printf("argv[%d] = %s\n", i, argv[i]);

        if(strcmp(argv[i],"|") != 0 
            && strcmp(argv[i],";") != 0 
            && strcmp(argv[i],"||") != 0 
            && strcmp(argv[i], "&&") != 0
            && i != argc-1){                        
                if(currBufferIndex == maxBuffSize){
                    buffer = realloc(buffer, currBufferIndex + maxBuffSize);
                    bufferCheckMallocOrRealloc(buffer);
                    maxBuffSize = currBufferIndex + maxBuffSize;
                }
                /*insire argv[i] no buffer*/
                buffer[currBufferIndex-1] = argv[i];
                currBufferIndex++;
        }

        /*PIPE/OR/AND cases*/
        else if(strcmp(argv[i], "|") == 0 || strcmp(argv[i], "||") == 0 || (strcmp(argv[i], "&&") == 0)){      
            /*insere os argumentos do buffer em um comando simples*/
            insertSimpleCommand(cmd, currBufferIndex-1, buffer);
            
            /*insere o operador na fila*/
            //printf("inserting %s on queue[%d]\n", argv[i], queueSize);
            insertElem(queue, argv[i]);
            queueSize++;

            /*limpa o buffer e reinicia o index para ler os proximos comandos*/
            clearBuffer(buffer, maxBuffSize);
            currBufferIndex = 1;
        }     

        /*independent commands case*/
        else if(strcmp(argv[i], ";") == 0){     
            /*insire os argumentos do buffer em um comando simples e executa a linha de comando atual*/
            insertSimpleCommand(cmd, currBufferIndex-1, buffer);
            executeCommand(cmd, queue);
            /*libera a linha de comando*/
            free(cmd);
            /*cria uma nova linha de comando*/        
            commandLine *cmd;
            cmd = (commandLine *)malloc(sizeof(commandLine*));
            cmdCheckMallocOrRealloc(cmd);
            initCommandLine(cmd);
            /*libera a fila e cria uma nova*/
            free(queue);
            elem *queue = initQueue();
            queueSize = 0;
            /*limpa o buffer e reinicia o index para ler os proximos comandos*/       
            clearBuffer(buffer, maxBuffSize);
            currBufferIndex = 1;
        }   

        /*end of command line*/
        else if(i == argc - 1){ 
            if(currBufferIndex == maxBuffSize){
                buffer = realloc(buffer, currBufferIndex + maxBuffSize);
                bufferCheckMallocOrRealloc(buffer);
                maxBuffSize = currBufferIndex + maxBuffSize;
            }
            /*insere os argumentos do buffer na linha de comando*/
            buffer[currBufferIndex-1] = argv[i];
            currBufferIndex++;
            insertSimpleCommand(cmd, currBufferIndex-1, buffer);
        }   
    }
    // int k=0;
    // elem *aux = queue;
    // while(aux->next != NULL){
    //     aux = aux->next;
    //     printf("queue[%d] = %s\n", k, aux->info);
    //     k++;
    // }        
    executeCommand(cmd, queue);
    free(buffer);
    free(cmd);
    free(queue);
    return 0;
}

void clearBuffer(char **buffer, int buffSize){
    int i;
    for(i =0; i < buffSize; i++){
        buffer[i] = NULL;
    }
}

void bufferCheckMallocOrRealloc(char **buffer){
    if(buffer == NULL){
        printf("allocation error\n");
        exit(EXIT_FAILURE);
    }    
}

void cmdCheckMallocOrRealloc(commandLine *cmd){
    if(cmd == NULL){
        printf("allocation error\n");
        exit(EXIT_FAILURE);
    }    
}

int executeCommand(commandLine *_command, elem *queue){
    printf("\nExecuting commands...\n");
    printf("Number of commands: %d\n", _command->_numberOfSimpleCommands); 
    if(isEmptyQueue(queue) != 1){
        char *operator = removeElem(queue);
        printf("operator = %s\n", operator);
    }
    int tmpin = dup(0);
    int tmpout = dup(1);

    int fdin;
    if(_command->_inpuFile){
        fdin = open(_command->_inpuFile, 0);
    } else {
        fdin = dup(tmpin);
    }

    pid_t child;

    int fdout;
    int status;

    for(int i = 0; i < _command->_numberOfSimpleCommands; i++){
        dup2(fdin, 0);
        close(fdin);

        if(i == _command->_numberOfSimpleCommands - 1){
            if(_command->_outFile){
                fdout = open(_command->_outFile, 0);
            }
            else{
                fdout = dup(tmpout);
            }
        }

        else{
            int fdpipe[2];
            pipe(fdpipe);
            fdout=fdpipe[1];
            fdin=fdpipe[0];
        }

        dup2(fdout, 1);
        close(fdout);

        child=fork();
        printf("currPID = %d\n", child);
        
        if(child == 0){
            printf("\nCommand: %s\n", _command->_simpleCommands[i]->_args[0]);
            for(int j=0; j < _command->_simpleCommands[i]->_numberOfArguments; j++){
                printf("Argument[%d]: %s\n", j, _command->_simpleCommands[i]->_args[j]);
            }
            execvp(_command->_simpleCommands[i]->_args[0], _command->_simpleCommands[i]->_args);
        }

        else if(child > 0){
            /*se existem operadores na fila*/
            // if(isEmptyQueue(queue) != 1){
            //     char *operator = removeElem(queue);
            //     printf("operator = %s\n", operator);
                
            //     if(strcmp(operator, "|") == 0){
            //         waitpid(child, &status, 0);
            //         //printf("filho terminou!\n");
            //     }

            //     if(strcmp(operator, "||") == 0){
            //         waitpid(child, &status, 0);
            //         if(status == 0){
            //             printf("filho executou!\n");
            //         }
            //     }

            //     if(strcmp(operator, "&&") == 0){
            //         waitpid(child, &status, 0);
            //         if(status == 0){
            //             printf("filho executou\n");
            //         }
            //     }
            // }
            // else{
            //     waitpid(child, &status, 0);
            //     printf("voltou ao pai\n"); 
            // }  
            waitpid(child, &status, 0);   
        }

        else {                    //error
            perror("fork()");
            exit(EXIT_FAILURE);
        }

        dup2(tmpin, 0);
        dup2(tmpout, 1);
        close(tmpin);
        close(tmpout);

        // if(!_command->_background){
        //     int status;
        //     waitpid(child, &status, 0);
        // }
    }
    printf("\n");
    return status;
}