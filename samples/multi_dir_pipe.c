#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
//#include <sys/wait.h>
#define STR_LEN 100

int main(void){
  int fd[2];                                                                    //file descriptor de duas posições
  int ch_fd[2];                                                                 //file descriptor do filho
  pid_t child;

  if(pipe(fd) < 0){
    perror("pipe()");
    return (-1);
  }
  if(pipe(ch_fd) < 0){
    perror("pipe(ch_fd)");
    return (-1);
  }

  child = fork();

  if(child == 0){                                                               //filho le no fd[0]
    char msg[STR_LEN];                                                          //cria um buffer
    close(fd[1]);                                                               //fecha fd[1] (escrita)
    close(ch_fd[0]);                                                            //fecha fd_ch[0] (leitura)   
    bzero(msg, STR_LEN);                                                        //limpa o buffer
    if(read(fd[0], msg, STR_LEN) > 0){                                          //le e salva a mensagem no buffer msg
      printf("Filho (PID=$%ld) leu: '%s'\n", (long)getpid(), msg);
    }
    bzero(msg, STR_LEN);
    sprintf(msg, "'Ok, pai!'");
    write(ch_fd[1], msg, strlen(msg));
    printf("Filho terminando ...\n");
    sleep(5);
    return 0;
  }
  else{                                                                         //pai escreve no fd[1]
    char pa_msg[STR_LEN];
    int status;
    close(fd[0]);                                                               //fecha descritor de index [0] (leitura)
    close(ch_fd[1]);
    bzero(pa_msg, STR_LEN);                                                     //limpa o buffer pa_msg
    printf("Pai (PID=$%ld) escrevendo para o filho...\n", (long)getpid());
    sprintf(pa_msg, "'Filho, preste atencao!'\n");                              //escreve no buffer pa_msg
    write(fd[1], pa_msg, strlen(pa_msg));                                       //escreve no pipe index [1] a msg
    bzero(pa_msg, STR_LEN);
    read(ch_fd[0], pa_msg, STR_LEN);
    printf('Pai leu: %s\n', pa_msg);
    waitpid(-1, &status, 0);                                                    //bloqueia ate o filho terminar
    printf("Pai: filho terminou...\n");
  }
  return 0;
}