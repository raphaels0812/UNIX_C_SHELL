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
/*
void executeCommand(commandLine *_command){
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
        if(child ==0){
            execvp(_command->_simpleCommands[i]->_args[0], _command->_simpleCommands[i]->_args);
            perror("fork()");
            _exit (1);
        }

        dup2(tmpin, 0);
        dup2(tmpout, 1);
        close(tmpin);
        close(tmpout);

        if(!_command->_background){
            int status;
            waitpid(child, &status, 0);
        }
    }
}
*/
/*
int main(int argc, char **argv){
        
    if(argc == 1){             
        printf("Uso: %s <comando> <p_1> <p_2> ... <p_n>\n", argv[0]);
        return 0;
    }

    for(int i = 0; i < argc; i++){
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    commandLine *cmdLine;
    initCommandLine(cmdLine);

    char **strAux = malloc(50*sizeof(char*));
    int j = 0;

    
    for(int i = 0; i < argc; i++){
        if(argv[i] != "|" && argv[i] != "\n"){
            strAux[j] = argv[i];
            printf("argv[%d] = %s\n", j, argv[j]);
            j++;
        }                                               //fim da linha
        else if(argv[i] == "\n"){
            strAux[j] = "\0";
            insertSimpleCommand(cmdLine, j, strAux);
        }
        else{                                           //encontrou pipe
            insertSimpleCommand(cmdLine, j, strAux);
            j = 0;
        }
    }

    printf("numberOfSimpleCommands = %d\n", cmdLine->_numberOfSimpleCommands);
    
    for(int i = 0; i < cmdLine->_numberOfSimpleCommands; i++){
        printf("command[%d] = %s\n", i, cmdLine->_simpleCommands[i]->_args[0]);
    }
    
}
*/