#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "commands.c"
#include "parser.c"

void clearBuffer(char **buffer, int buffSize);
void checkMallocOrRealloc(char **buffer);
void executeCommand(commandLine *_command);

int main(int argc, char **argv){
        if(strcmp(argv[0], "exit") == 0){
            return 0;
        } 

        if(argc == 1 && strcmp(argv[0], "ls") != 0){             
            printf("Correct use: %s <command> <arg1> <arg2> ... <argn>\n", argv[0]);
        }

        else{
            int maxBuffSize = 10;
            char **buffer = (char **)malloc(maxBuffSize*sizeof(char**));
            checkMallocOrRealloc(buffer);

            int currBuffSize = 1;
            int j = 0;

            commandLine *cmd;
            cmd = (commandLine *)malloc(sizeof(commandLine*));
            initCommandLine(cmd);
            
            for(int i = 0; i < argc; i++){

                if(strcmp(argv[i],"|")!=0 && i!=argc-1 && strcmp(argv[i],";")!=0 && strcmp(argv[i],"||")!=0 && strcmp(argv[i], "&&")!=0){
                    
                    if(currBuffSize == maxBuffSize){
                        buffer = realloc(buffer, currBuffSize + maxBuffSize);
                        checkMallocOrRealloc(buffer);
                        maxBuffSize = currBuffSize + maxBuffSize;
                    }

                    buffer[j] = argv[i];
                    currBuffSize++;
                    j++;
                }

                else if(strcmp(argv[i], "|") == 0){    //encontrou pipe                                                  
                    // for(int k=0; k<currBuffSize-1; k++){
                    //     printf("buffer[%d] = %s\n", k, buffer[k]);
                    // }
                    insertSimpleCommand(cmd, currBuffSize-1, buffer);
                    clearBuffer(buffer, maxBuffSize);
                    currBuffSize = 1;
                    j = 0;
                }      

                else if(strcmp(argv[i], ";") == 0){     //encontrou ";" - comandos independentes
                    insertSimpleCommand(cmd, currBuffSize-1, buffer);
                    executeCommand(cmd);
                    free(cmd);
                    
                    commandLine *cmd;
                    cmd = (commandLine *)malloc(sizeof(commandLine*));
                    initCommandLine(cmd);
                    
                    clearBuffer(buffer, maxBuffSize);
                    currBuffSize = 1;
                    j = 0;
                }

                else if(strcmp(argv[i], "||") == 0){        //encontrou "||" = OR

                }   

                else if(strcmp(argv[i], "&&") == 0){        //encontrou "&&" = AND

                }

                else if(i == argc - 1){  //fim da linha
                    if(currBuffSize == maxBuffSize){
                        buffer = realloc(buffer, currBuffSize + maxBuffSize);
                        checkMallocOrRealloc(buffer);
                        maxBuffSize = currBuffSize + maxBuffSize;
                    }

                    buffer[j] = argv[i];
                    currBuffSize++;
                    // for(int k=0; k<currBuffSize-1; k++){
                    //     printf("buffer[%d] = %s\n", k, buffer[k]);
                    // }
                    insertSimpleCommand(cmd, currBuffSize-1, buffer);
                }   
            }
            executeCommand(cmd);
            free(buffer);
            free(cmd);
        }
}

void clearBuffer(char **buffer, int buffSize){
    int i;
    for(i =0; i < buffSize; i++){
        buffer[i] = NULL;
    }
}

void checkMallocOrRealloc(char **buffer){
    if(buffer == NULL){
        printf("allocation error\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }    
}

void executeCommand(commandLine *_command){
    //printf("\nExecuting commands ...\n");
    //printf("Number of commands: %d\n", _command->_numberOfSimpleCommands);
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
            //printf("\nCommand: %s\n", _command->_simpleCommands[i]->_args[0]);
            execvp(_command->_simpleCommands[i]->_args[0], _command->_simpleCommands[i]->_args);
        }

        else if(child > 0){
            int status;
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
}