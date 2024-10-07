#include "redirections.h"

/* traitement du tube anonyme -> WARNING : son éxécution provoque un exit du prompt */
int tube_anonyme(char * command){
  int fd[2];
  char * argv[BUFFER_SIZE];
  char * token;
  token = strtok(command, "|");
  int c = 0;
  
  /* chaque commande entre "|" devient un élément du tableau argv */
  while(token!=NULL){
    argv[c] = token;
    token = strtok(NULL, "|");
    c++;
  }
  argv[c]=NULL;

  /* à présent chaque commande est découpé dans un nouveau tableau char ** avec chaque élément correspondant à un argument de la commande */
  char *cmd[c][MAX_ARGS_NUMBER+1];
  int t;
  for(int i=0;i<c;i++){
    token = strtok(argv[i], " ");
    t = 0;
    while(token!=NULL){
      cmd[i][t] = token;
      token = strtok(NULL, " ");
      t++;
    }
    cmd[i][t]=NULL;
    t = 0;
  }

  int precpipe = STDIN_FILENO; /* on conseve le pipe précédent pour garder sa trace */
  pid_t pid;
  for(int i=0;i<c-1;i++){
    /* erreur */
    if(pipe(fd)==-1){
      printf("slash: pipe error\n");
      return 1;
    }
    pid = fork();
    /* erreur */
    if(pid<0){
      printf("slash: fork error\n");
      return 1;
    }
    /* le fils remplace le processus courant par la commande i */
    if(pid==0){
      if(precpipe!=STDIN_FILENO){
	  dup2(precpipe, STDIN_FILENO);
	  close(precpipe);
      }
      dup2(fd[1], STDOUT_FILENO);
      close(fd[1]);
      execvp(cmd[i][0], cmd[i]);
      return 1;
    }
    close(precpipe);
    close(fd[1]);
    precpipe=fd[0];
  }
  if(precpipe!=STDIN_FILENO){
    dup2(precpipe, STDIN_FILENO);
    close(precpipe);
  }
  /* on remplace le processus par la commande précédente */
  execvp(cmd[c-1][0], cmd[c-1]);
  return 0;
}

/* traitement des tubes nommés */
int tube_nomme(char *cmd, char *fic, char *r){
  int fd;
  char * argv[BUFFER_SIZE];
  char * token;
  token = strtok(cmd, " ");
  int c = 0;
    
  /* tous les arguments de la commande sont stockés dans un tableau */
  while(token!=NULL){
    argv[c] = token;
    token = strtok(NULL, " ");
    c++;
  }
  argv[c]=NULL;
    
  /* en fonction de la redirection voulu, on donne des flags différents à open */
  if(!strcmp(r, "<")){
    fd = open(fic+1, O_RDONLY);
    /* STDOUT_FILENO devient une copie de fd */
    dup2(fd, 0);
    /* on remplace le processus */
    execvp(argv[0], argv);
  }
  if(!strcmp(r, ">|") || !strcmp(r, "2>|")){
    fd = open(fic+1, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP);
    /* la duplication dépend de si on on redirige une erreur standard ou non */
    !strcmp(r, ">|") ? dup2(fd, 1) : dup2(fd, 2);
  }
  if(!strcmp(r, ">>") || !strcmp(r, "2>>")){
    fd = open(fic+1, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP);
    !strcmp(r, ">>") ? dup2(fd, 1) : dup2(fd, 2);
  }
  if(!strcmp(r, ">") || !strcmp(r, "2>")){
    fd = open(fic+1, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP);
    !strcmp(r, ">") ? dup2(fd, 1) : dup2(fd, 2);
  }
  pid_t pid = fork();
  /* erreur */
  if(pid<0){
    printf("slash: pipe error\n");
    return 1;
  /* le fils ferme le descripteur et remplace le processus */
  }else if(pid==0){
  close(fd);
  execvp(argv[0], argv);
  _exit(1);
  /* le père attend son fils*/
  }else{
    wait(NULL);
  }
  return 0;
}
