#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "commands.c"
#include "queue.c"

void clearBuffer(char **buffer, int buffSize);
void bufferCheckMallocOrRealloc(char **buffer);
void cmdCheckMallocOrRealloc(commandLine *cmd);
int execute(commandLine *_command, int i);
int executeWithPipe(commandLine *_command, int i);

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

        /*PIPE/OR/AND/SEMICOLON cases*/
        else if(strcmp(argv[i], "|") == 0 
            || strcmp(argv[i], "||") == 0 
            || strcmp(argv[i], "&&") == 0
            || strcmp(argv[i], ";") == 0){      
            /*insere os argumentos do buffer em um comando simples*/
            insertSimpleCommand(cmd, currBufferIndex-1, buffer);
            
            /*insere o operador na fila*/
            insertElem(queue, argv[i]);
            queueSize++;

            /*limpa o buffer e reinicia o index para ler os proximos comandos e argumentos*/
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

    char *lastOperator = "NULL";

    for(int i = 0; i < cmd->_numberOfSimpleCommands; i++){
        /*se a fila nao estiver vazia*/
        if(isEmptyQueue(queue) != 1){
            /*remove primeiro elemento da fila*/
            char *operator = removeElem(queue);
            
            /*executa comando com PIPE (escreve em fdin)*/
            if(strcmp(operator, "|") == 0){
                lastOperator = operator;
                executeWithPipe(cmd, i);
            }

            /*executa comando simples*/
            else if(strcmp(operator, ";") == 0){
                lastOperator = operator;
                execute(cmd, i);
            }

            /*executa comando i OR i+1*/
            else if(strcmp(operator, "||") == 0){
                if(execute(cmd, i) == 0 || execute(cmd, i+1) == 0){
                    lastOperator = operator;
                    i++;
                }
                else{
                    return -1;
                }
            }

            /*executa comando i AND i+1*/
            else if(strcmp(operator, "&&") == 0){
                if(execute(cmd, i) == 0 && execute(cmd, i+1) == 0){
                    lastOperator = operator;
                    i++;
                }
                else{
                    return -1;
                }
            }
        }
        /*executa comando com PIPE (le conteudo de fdout)*/
        else if(strcmp(lastOperator, "|") == 0){
            executeWithPipe(cmd, i);
        }
        else{
            execute(cmd, i);
        }
    }       
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

/*executa um comando simples*/
int execute(commandLine *_command, int i){
    pid_t child;
    int status;

    child = fork();
    
    if(child == 0){
        execvp(_command->_simpleCommands[i]->_args[0], _command->_simpleCommands[i]->_args);
    }

    else if(child > 0){
        waitpid(child, &status, 0);
    }

    else{
        perror("fork()");
        exit(EXIT_FAILURE);
    }

    return status;
}

/*executa comando com combinacao de entrada/saida*/
int executeWithPipe(commandLine *_command, int i){
    pid_t child;
    int status;
    
    /*fd de escrita*/
    int fdin;

    /*fd de leitura*/
    int fdout;

    /*copia temporaria de fd[0]*/
    int tmpin = dup(0);

    /*copia temporaria de fd[1]*/
    int tmpout = dup(1);
    
    /*escreve o conteudo do fd de escrita no canal [0] e fecha*/
    dup2(fdin, 0);
    close(fdin);

    if(i == _command->_numberOfSimpleCommands - 1){
        fdout = dup(tmpout);
    }

    else {
        /*PIPE de comunicacao*/
        int fdpipe[2];
        
        if(pipe(fdpipe) < 0){
            perror("pipe()");
            return -1;
        }

        fdout = fdpipe[1];
        fdin = fdpipe[0];
    }  

    /*copia o conteudo da saida em [1] e fecha*/
    dup2(fdout, 1);
    close(fdout);

    child = fork();
    
    if(child == 0){
        execvp(_command->_simpleCommands[i]->_args[0], _command->_simpleCommands[i]->_args);
    }

    else if(child > 0){
        waitpid(child, &status, 0);
    }

    else{
        perror("fork()");
        exit(EXIT_FAILURE);
    }
    /*copia o conteudo do fd temporario de entrada em fd[0]*/
    dup2(tmpin, 0);
    /*copia o conteudo do fd temporario de saida em fd[1]*/
    dup2(tmpout, 1);
    
    close(tmpin);
    close(tmpout);

    return status;
}
