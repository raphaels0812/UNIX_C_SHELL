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

void clearBuffer(char **buffer, int buffSize);
void bufferCheckMallocOrRealloc(char **buffer);
void cmdCheckMallocOrRealloc(commandLine *cmd);
int executeCommand(commandLine *_command);

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
    
    /*pilha para armazenar o resultado dos comandos com operadores AND e OR*/
    node *stack = initStack();
    int stackSize = 0;           
    
    /*itera sobre os itens da matriz de argumentos (argv)*/
    for(int i = 1; i < argc; i++){
        
        //printf("argv[%d] = %s\n", i, argv[i]);

        if(strcmp(argv[i],"|") != 0 
            && i != argc-1 
            && strcmp(argv[i],";") != 0 
            && strcmp(argv[i],"||") != 0 
            && strcmp(argv[i], "&&") != 0){                        
                if(currBufferIndex == maxBuffSize){
                    buffer = realloc(buffer, currBufferIndex + maxBuffSize);
                    bufferCheckMallocOrRealloc(buffer);
                    maxBuffSize = currBufferIndex + maxBuffSize;
                }
                /*insiro argv[i] no buffer*/
                buffer[currBufferIndex-1] = argv[i];
                currBufferIndex++;
        }

        /*PIPE case*/
        else if(strcmp(argv[i], "|") == 0){                                                     
            /*insere os argumentos do buffer em um comando simples*/
            insertSimpleCommand(cmd, currBufferIndex-1, buffer);
            /*limpa o buffer e reinicia o index para ler os proximos comandos*/
            clearBuffer(buffer, maxBuffSize);
            currBufferIndex = 1;
        }      

        /*independent commands case*/
        else if(strcmp(argv[i], ";") == 0){     
            /*insire os argumentos do buffer em um comando simples e executa a linha de comando atual*/
            insertSimpleCommand(cmd, currBufferIndex-1, buffer);
            executeCommand(cmd);
            /*libera a linha de comando atual*/
            free(cmd);
            /*cria uma nova linha de comando*/        
            commandLine *cmd;
            cmd = (commandLine *)malloc(sizeof(commandLine*));
            cmdCheckMallocOrRealloc(cmd);
            initCommandLine(cmd);
            /*limpa o buffer e reinicia o index para ler os proximos comandos*/       
            clearBuffer(buffer, maxBuffSize);
            currBufferIndex = 1;
        }

        /*OR case*/
        else if(strcmp(argv[i], "||") == 0){      
            /*insere os argumentos do buffer em um comando simples e executa a linha de comando*/
            insertSimpleCommand(cmd, currBufferIndex-1, buffer);
            int status = executeCommand(cmd);
            
            /*comando simples falhou, permite a execucao do proximo comando*/
            if(status != 0){        
                /*libera a linha de comando e cria uma nova linha*/
                free(cmd);        
                commandLine *cmd;
                cmd = (commandLine *)malloc(sizeof(commandLine*));
                cmdCheckMallocOrRealloc(cmd);
                initCommandLine(cmd);
                /*limpa o buffer e reincia o index*/     
                clearBuffer(buffer, maxBuffSize);
                currBufferIndex = 1;
            }

            /*comando simples executou, mas ainda existem outros comandos na matriz de argumentos*/
            else if(status == 0){
                /*armazena o status na pilha*/
                push(stack, status);
                stackSize++;
                /*libera a linha de comando e cria uma nova linha*/
                free(cmd);        
                commandLine *cmd;
                cmd = (commandLine *)malloc(sizeof(commandLine*));
                cmdCheckMallocOrRealloc(cmd);
                initCommandLine(cmd);
                /*limpa o buffer e reincia o index*/        
                clearBuffer(buffer, maxBuffSize);
                currBufferIndex = 1;
            }

            /*comando simples executou com sucesso, entao encerra*/
            else{
                exit(EXIT_SUCCESS);
            }
        }   

        /*AND case*/
        else if(strcmp(argv[i], "&&") == 0){      
            /*insere os argumentos do buffer em um comando simples e executa a linha de comando*/
            insertSimpleCommand(cmd, currBufferIndex-1, buffer);
            int status = executeCommand(cmd);

            /*comando simples executou com sucesso, permite a execucao do proximo comando*/
            if(status == 0){        
                free(cmd);        
                commandLine *cmd;
                cmd = (commandLine *)malloc(sizeof(commandLine*));
                cmdCheckMallocOrRealloc(cmd);
                initCommandLine(cmd);
                /*limpa o buffer e reincia o index*/        
                clearBuffer(buffer, maxBuffSize);
                currBufferIndex = 1;
            }

            /*comando simples falhou mas ainda existem outros comandos*/
            else if(status != 0 && i < argc-1){
                /*armazeno o status na pilha*/
                push(stack, status);
                stackSize++;
                /*libera a linha de comando e cria uma nova linha*/
                free(cmd);        
                commandLine *cmd;
                cmd = (commandLine *)malloc(sizeof(commandLine*));
                cmdCheckMallocOrRealloc(cmd);
                initCommandLine(cmd);
                /*limpa o buffer e reincia o index*/        
                clearBuffer(buffer, maxBuffSize);
                currBufferIndex = 1;
            }

            /*comando simples falhou, entao encerra*/
            else{
                exit(EXIT_FAILURE);
            }
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
    executeCommand(cmd);
    free(buffer);
    free(cmd);
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

int executeCommand(commandLine *_command){
    printf("\nExecuting commands...\n");
    printf("Number of commands: %d\n", _command->_numberOfSimpleCommands);
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

    for(int i=0; i < _command->_numberOfSimpleCommands; i++){
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
        
        if(child == 0){
            printf("\nCommand: %s\n", _command->_simpleCommands[i]->_args[0]);
            for(int j=0; j < _command->_simpleCommands[i]->_numberOfArguments; j++){
                printf("Argument[%d]: %s\n", j, _command->_simpleCommands[i]->_args[j]);
            }
            execvp(_command->_simpleCommands[i]->_args[0], _command->_simpleCommands[i]->_args);
        }

        else if(child > 0){
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