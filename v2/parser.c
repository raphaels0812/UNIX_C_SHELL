/*break up the individual parts of the command line input*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DELIMETERS " \"\t\r\n\a"

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
    char **tokens = malloc(buffSize * sizeof(char**));

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

    return tokens;
}