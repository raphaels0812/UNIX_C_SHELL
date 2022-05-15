#ifndef MY_SHELL_H
# define MY_SHELL_H
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# include <unistd.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/wait.h>

# define DELIMETERS " \"\t\r\n\a"

# define DEFAULT_NUM_OF_ARGS 5
# define DEFAULT_NUM_OF_COMMANDS 5

/*struct of a simple command*/
typedef struct{
    int _numberOfAvailableArguments;
    int _numberOfArguments;
    char **_args;
}	simpleCommand;

/*struct of a shell command line, containing 1 or more simple commands*/
typedef struct{
    int _numberOfAvailableSimpleCommands;
    int _numberOfSimpleCommands;
    simpleCommand **_simpleCommands;
    char *_outFile;
    char *_inpuFile;
    char *_errFile;
    int _background;
}	commandLine;

//commands.c
void	initSimpleCommand(simpleCommand *_simpleCommand);
void	insertArgument(simpleCommand *_simpleCommand, char *argument);
void	initCommandLine(commandLine *_commandLine);
void	insertSimpleCommand(commandLine *_commandLine, int _numberOfArgs, char **args);

//parser.c
char	*readLine(void);
char	**splitLine(char *line);
int 	splitLineLen(char **argv);

//queue.c
typedef struct Elem{
    char	*info;
    struct	Elem *next;
}	elem;

elem	*initQueue();
int		isEmptyQueue(elem *queue);
void	insertElem(elem *queue, char *info);
char	*removeElem(elem *queue);
void	printQueue(elem *queue);

#endif
