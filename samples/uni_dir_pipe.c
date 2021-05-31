#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
//#include <sys/wait.h>
#define STR_LEN 100

int main(void){
  int fd[2];                                                                    //file descriptor de duas posições
  pid_t child;

  if(pipe(fd) < 0){
    perror("pipe()");
    return -1;
  }

  child = fork();

  if(child == 0){                                                               //filho le no fd[0]
    char msg[STR_LEN];
    close(fd[1]);                                                               //fecha descritor de index [1] (escrita)
    bzero(msg, STR_LEN);                                                        //limpa o buffer
    if(read(fd[0], msg, STR_LEN) > 0){                                          //le e salva a mensagem no buffer msg
      printf("Filho (PID=$%ld) leu: '%s'\n", (long)getpid(), msg);
    }
    printf("Filho terminando ...\n");
    sleep(5);
    return 0;
  }
  else{                                                                         //pai escreve no fd[1]
    char pa_msg[STR_LEN];
    int status;
    close(fd[0]);                                                               //fecha descritor de index [0] (leitura)
    bzero(pa_msg, STR_LEN);                                                     //limpa o buffer pa_msg
    printf("Pai (PID=$%ld) escrevendo para o filho...\n", (long)getpid());
    sprintf(pa_msg, "'Filho, preste atencao!'\n");                                //escreve no buffer pa_msg
    write(fd[1], pa_msg, strlen(pa_msg));                                       //escreve no pipe index [1] a msg
    waitpid(-1, &status, 0);                                                    //bloqueia ate o filho terminar
    printf("Pai: filho terminou...\n");
  }
  return 0;
}