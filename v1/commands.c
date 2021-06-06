#include <stdlib.h>
#include <stdio.h>
#define DEFAULT_NUM_OF_ARGS 5
#define DEFAULT_NUM_OF_COMMANDS 5

typedef struct{
    int _numberOfAvailableArguments;
    int _numberOfArguments;
    char **_args;
}simpleCommand;

typedef struct{
    int _numberOfAvailableSimpleCommands;
    int _numberOfSimpleCommands;
    simpleCommand **_simpleCommands;  /*Command table*/ 
    char *_outFile;
    char *_inpuFile;
    char *_errFile;
    int _background;
}commandLine;

void initSimpleCommand(simpleCommand *_simpleCommand){
    _simpleCommand->_numberOfAvailableArguments = DEFAULT_NUM_OF_ARGS;
    _simpleCommand->_numberOfArguments = 0;
    _simpleCommand->_args = (char **)malloc(DEFAULT_NUM_OF_ARGS*sizeof(char*));
    if(_simpleCommand->_args == NULL){
        printf("initSimpleCommand: allocation error\n");
        exit(EXIT_FAILURE);
    }
}

void insertArgument(simpleCommand *_simpleCommand, char *argument){
    if(_simpleCommand->_numberOfAvailableArguments > 0){
        _simpleCommand->_args[_simpleCommand->_numberOfArguments] = argument;
        _simpleCommand->_numberOfArguments += 1;
        _simpleCommand->_numberOfAvailableArguments -= 1;
    }
    else{
        _simpleCommand->_args = (char **)realloc(_simpleCommand->_args, (_simpleCommand->_numberOfArguments + DEFAULT_NUM_OF_ARGS)*sizeof(char*));
        if(!_simpleCommand->_args){
            fprintf(stderr, "insertArgument: reallocation error\n");
            exit(EXIT_FAILURE);
        }
        else{
            _simpleCommand->_numberOfAvailableArguments = DEFAULT_NUM_OF_ARGS;
            _simpleCommand->_args[_simpleCommand->_numberOfArguments] = argument;
            _simpleCommand->_numberOfArguments += 1;
            _simpleCommand->_numberOfAvailableArguments -= 1;
        }
        
    }
}

void initCommandLine(commandLine *_commandLine){
    _commandLine->_numberOfAvailableSimpleCommands = DEFAULT_NUM_OF_COMMANDS;
    _commandLine->_numberOfSimpleCommands = 0;

    _commandLine->_simpleCommands = (simpleCommand **)malloc(DEFAULT_NUM_OF_COMMANDS*sizeof(simpleCommand*));
    if(_commandLine->_simpleCommands == NULL){
        printf("initCommandLine: allocation error\n");
        exit(EXIT_FAILURE);
    }
    _commandLine->_outFile = NULL;
    _commandLine->_inpuFile = NULL;
    _commandLine->_errFile = NULL;
    _commandLine->_background = 0;
}

void insertSimpleCommand(commandLine *_commandLine, int _numberOfArgs, char **args){
    if(_commandLine->_numberOfAvailableSimpleCommands > 0){
        simpleCommand *newSimpleCommand;
        newSimpleCommand = (simpleCommand *)malloc(sizeof(simpleCommand*));
        
        if(newSimpleCommand == NULL){
            printf("insertSimpleCommand: allocation error\n");
            exit(EXIT_FAILURE);
        }

        initSimpleCommand(newSimpleCommand);
        newSimpleCommand->_numberOfArguments = _numberOfArgs;
        
        for(int i = 0; i < _numberOfArgs; i++){
            insertArgument(newSimpleCommand, args[i]);
        }

        _commandLine->_simpleCommands[_commandLine->_numberOfSimpleCommands] = newSimpleCommand;
        _commandLine->_numberOfSimpleCommands += 1;
        _commandLine->_numberOfAvailableSimpleCommands -= 1;
    }
    else{
        _commandLine->_simpleCommands = (simpleCommand **)realloc(_commandLine->_simpleCommands, (_commandLine->_numberOfSimpleCommands + DEFAULT_NUM_OF_COMMANDS)*sizeof(simpleCommand*));
        
        if(_commandLine->_simpleCommands == NULL){
            fprintf(stderr, "insertSimpleCommand: allocation error\n");
            exit(EXIT_FAILURE);
        }
        else{
            _commandLine->_numberOfAvailableSimpleCommands = DEFAULT_NUM_OF_COMMANDS;
            simpleCommand *newSimpleCommand;
            newSimpleCommand = (simpleCommand *)malloc(sizeof(simpleCommand*));
        
            if(newSimpleCommand == NULL){
                printf("insertSimpleCommand: allocation error\n");
                exit(EXIT_FAILURE);
            }

            initSimpleCommand(newSimpleCommand);
            newSimpleCommand->_numberOfArguments = _numberOfArgs;
            
            for(int i = 0; i < _numberOfArgs; i++){
                insertArgument(newSimpleCommand, args[i]);
            }

            _commandLine->_simpleCommands[_commandLine->_numberOfSimpleCommands] = newSimpleCommand;
            _commandLine->_numberOfSimpleCommands += 1;
            _commandLine->_numberOfAvailableSimpleCommands -= 1;
        }    
    }
}

int main(int argc, char **argv){
    commandLine *cmd = malloc(sizeof(commandLine));
    if(cmd == NULL){
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }
    initCommandLine(cmd);
    insertSimpleCommand(cmd, argc, argv);
    printf("teste\n");
}