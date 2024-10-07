#include "cmd_externes.h"

/* traitement des commandes externes */
int cmd_externe(char **argv){
    pid_t pid = fork();
    pid_t w;
    /* erreur */
    if(pid<0){
      return 1;
    /* action du fils -> remplacement du processus en cours */
    } else if(pid==0) {
      execvp(argv[0], argv);
      _exit(127);
    /* action du père -> attend son fils puis renvoie sa valeur de retour */
    } else {
      int status;
      w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
      if(w==-1){
	return 1;
	}
    const int s = WEXITSTATUS(status);
    if(s == 127) return 127;
    
    if(WIFEXITED(status)){
      return s;
      }
    return 0;
  }
  
  return 127;
}
