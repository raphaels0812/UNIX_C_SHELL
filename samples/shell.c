#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
//#include <sys/wait.h>

int main(int argc, char **argv){
  pid_t pid;
  if(argc == 1){              //uso inválido, necessário fornecer comando e argumentos
    printf("Uso: %s <comando> <p_1> <p_2> ... <p_n>\n", argv[0]);
    return 0;
  }

  pid = fork();               //cria processo filho

  if(pid == 0){               //processo filho: executa o comando
    char **cmd;               //ponteiro para a matriz de argumentos
    cmd = &argv[1];           //ponteiro aponta para argv[1], eliminando argv[0] (chamada do shell)
    execvp(cmd[0], cmd);      //subsitui a imagem do processo por outra, passada como primeiro argumento 
  } 
  else if(pid >= 0){        //processo pai: aguarda o processo filho
    int status;
    waitpid(-1, &status, 0);  //aguarda o PID do filho retornar status concluído
  } 
  else {                    //error
    perror("fork()");
    return -1;
  }
  return 0;
}