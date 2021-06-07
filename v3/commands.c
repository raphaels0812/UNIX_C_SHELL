#include <stdlib.h>
#include <stdio.h>
#define DEFAULT_NUM_OF_ARGS 5
#define DEFAULT_NUM_OF_COMMANDS 5

/*struct of a simple command*/
typedef struct{
    int _numberOfAvailableArguments;
    int _numberOfArguments;
    char **_args;
}simpleCommand;

/*struct of a shell command line, containing 1 or more simple commands*/
typedef struct{
    int _numberOfAvailableSimpleCommands;
    int _numberOfSimpleCommands;
    simpleCommand **_simpleCommands;  
    char *_outFile;
    char *_inpuFile;
    char *_errFile;
    int _background;
}commandLine;

/*"constructor" of simple command struct*/
void initSimpleCommand(simpleCommand *_simpleCommand){
    _simpleCommand->_numberOfAvailableArguments = DEFAULT_NUM_OF_ARGS;
    _simpleCommand->_numberOfArguments = 0;
    _simpleCommand->_args = (char **)malloc(DEFAULT_NUM_OF_ARGS*sizeof(char**));
    if(_simpleCommand->_args == NULL){
        printf("initSimpleCommand: allocation error\n");
        exit(EXIT_FAILURE);
    }
}

/*insert an argument on a simple command struct*/
void insertArgument(simpleCommand *_simpleCommand, char *argument){
    //printf("inserting simpleCommand[].argument[%d] = %s\n", _simpleCommand->_numberOfArguments, argument);
    if(_simpleCommand->_numberOfAvailableArguments > 0){
        _simpleCommand->_args[_simpleCommand->_numberOfArguments] = argument;
        _simpleCommand->_numberOfArguments += 1;
        _simpleCommand->_numberOfAvailableArguments -= 1;
    }
    else{
        _simpleCommand->_args = (char **)realloc(_simpleCommand->_args, (_simpleCommand->_numberOfArguments + DEFAULT_NUM_OF_ARGS)*sizeof(char**));
        if(_simpleCommand->_args == NULL){
            printf("insertArgument: reallocation error\n");
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

/*"constructor" of a command line struct*/
void initCommandLine(commandLine *_commandLine){
    _commandLine->_numberOfAvailableSimpleCommands = DEFAULT_NUM_OF_COMMANDS;
    _commandLine->_numberOfSimpleCommands = 0;

    _commandLine->_simpleCommands = (simpleCommand **)malloc(DEFAULT_NUM_OF_COMMANDS*sizeof(simpleCommand**));
    if(_commandLine->_simpleCommands == NULL){
        printf("initCommandLine: allocation error\n");
        exit(EXIT_FAILURE);
    }
    _commandLine->_outFile = NULL;
    _commandLine->_inpuFile = NULL;
    _commandLine->_errFile = NULL;
    _commandLine->_background = 0;
}

/*insert a single command struct on a command line struct*/
void insertSimpleCommand(commandLine *_commandLine, int _numberOfArgs, char **args){
    if(_commandLine->_numberOfAvailableSimpleCommands > 0){
        simpleCommand *newSimpleCommand;
        newSimpleCommand = (simpleCommand *)malloc(sizeof(simpleCommand*));
        
        if(newSimpleCommand == NULL){
            printf("insertSimpleCommand: allocation error\n");
            exit(EXIT_FAILURE);
        }

        initSimpleCommand(newSimpleCommand);
        
        for(int i = 0; i < _numberOfArgs; i++){
            insertArgument(newSimpleCommand, args[i]);
        }

        _commandLine->_simpleCommands[_commandLine->_numberOfSimpleCommands] = newSimpleCommand;
        _commandLine->_numberOfSimpleCommands += 1;
        _commandLine->_numberOfAvailableSimpleCommands -= 1;
    }
    else{
        _commandLine->_simpleCommands = (simpleCommand **)realloc(_commandLine->_simpleCommands, (_commandLine->_numberOfSimpleCommands + DEFAULT_NUM_OF_COMMANDS)*sizeof(simpleCommand**));
        
        if(_commandLine->_simpleCommands == NULL){
            printf("insertSimpleCommand: allocation error\n");
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