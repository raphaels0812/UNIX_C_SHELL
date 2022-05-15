/*break up the individual parts of the command line input*/
#include "my_shell.h"

char *readLine(void){
    char *line = NULL;
    ssize_t buffSize = 0;

    if(getline(&line, &buffSize, stdin) == -1){
        if(feof(stdin)){
            fflush(stdin);
            exit(0);
        }
        else{
            perror("readLine");
            exit(-1);
        }
    }
    return line;
}

char **splitLine(char *line){
    int buffSize = 64;
    int position = 0;
    char **tokens = malloc(buffSize * sizeof(char*));

    if(!tokens){
        fprintf(stderr, "allocation error\n");
        exit(-1);
    }

    char *token;
    token = strtok(line, DELIMETERS);

    while(token != NULL){
        tokens[position] = token;
        position++;

        if(position >= buffSize){
            buffSize += 64;
            tokens = realloc(tokens, buffSize * sizeof(char*));
            if(!tokens){
                fprintf(stderr, "allocation error\n");
                exit(-1);
            }
        }
        token = strtok(NULL, DELIMETERS);
    }
     if(position >= buffSize){
        buffSize += 64;
        tokens = realloc(tokens, buffSize * sizeof(char*));
        if(!tokens){
            fprintf(stderr, "allocation error\n");
            exit(-1);
        }
    }
    tokens[position] = NULL;
    return tokens;
}

int splitLineLen(char **argv)
{
    int i;

    i = 0;
    while (argv[i])
        i++;
    return i;
}
