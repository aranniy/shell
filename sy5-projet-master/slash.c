#include "slash.h"
#include "cmd_internes.h"
#include "cmd_externes.h"
#include "redirections.h"


int val_retour = 0; /* valeur de retour de la commande tapé par l'utilisateur */

/* permet de vérifier que la valeur censée être renvoyé par le exit est un nombre */
int verif_numbers (char * nombre) {
    
    size_t length = strlen(nombre);
    
    for( int i=0; i<length; i++) {
        if (!isdigit(nombre[i])) 
            return 1;
    }
    
    return 0;
    
}

/* commande exit */
int cmd_exit(char * token){

    /* si la valeur de retour n'est pas mentionné, on renvoie la dernière obtenu */
    if (token == NULL){
        exit(val_retour);
    }

    /* si la valeur de retour est mentionné ET est compatible, on exit avec cette valeur */
    else if (!verif_numbers(token)){
        exit(atoi(token));
    }
    
    else
        printf("exit prend un nombre en argument...\n");
    
    return 1;
}

/* on vérifie que s1 se finit par s2 */
int endWith(char *s1, char *s2) {
    return (!strcmp(s1,s2)) || (strlen(s1) >= strlen(s2) && (!strcmp(s1+ strlen(s1) - strlen(s2),s2)));
}

/* traitement de la commande de l'utilisateur */
int traitement_commande(char *ligne,char **argv) {
    
    char * token;
    token = strtok(ligne, " ");
    
    if (!strcmp(token,"exit"))
        return cmd_exit(strtok(NULL," "));
        
    else if (!strcmp(token,"cd"))
        return cmd_cd(strtok(NULL, " "));
        
    else if (!strcmp(token,"pwd"))
        return cmd_pwd(strtok(NULL, " "));
    else{
        return cmd_externe(argv);
    }
    
}

/* ajoute s à la dernière position du tableau tab */
void ajouter (char* s, char* tab[]){
    int i = 0;
    while(tab[i] != NULL) {
        i++;
    }
    tab[i] = s;
    tab[i+1] = NULL;
}

/* gestion du joker étoile -> WARNING : parcours incorrecte, fuites de mémoire et provoque une boucle infini lors des test */
void etoile(char * chemin, char** arg, char * tab[], int i){
    char * token = arg[i];
    
    /* si l'élément i du tableau est NULL */
    if(token == NULL){
        if(strlen(chemin) > 1 && chemin[0]=='/' && chemin[1]=='/'){
            char * s = malloc(strlen(chemin));
            strcpy(s, chemin+1);
            ajouter(s, tab);
            free(chemin);
        }else{
            ajouter(chemin, tab);
        }
    }
    
    /* si le premier caractère n'est pas une étoile */
    else if(token[0] != '*'){
        if(chemin == NULL){
            chemin = malloc(MAX_ARGS_STRLEN);
            chemin[0] = '\0';
        }else{
            strcat(chemin, "/");
        }

        DIR * d = opendir(chemin);
        int n = 0;
        if(d != NULL){
            struct dirent *entry;
            while ((entry = readdir(d))) {
                if(!strcmp(entry -> d_name,token))
                    n = 1;
            }
        }else{
            n = 1;
        }
        if(n){
            strcat(chemin, token);
            etoile(chemin, arg, tab, i+1);
        }else{
            free(chemin);
        }
        free(d);
    }
    
    /* si le premier caractère est une étoile */
    else {
        DIR * dir;
        if(chemin == NULL){
            dir = opendir(".");
            chemin = malloc(MAX_ARGS_STRLEN);
            chemin[0] = '\0';
        }else{
            dir = opendir(chemin);
            if(chemin[strlen(chemin)-1] != '/'){
                strcat(chemin, "/");
            }
        }     

        struct dirent *entry;
        
        while ((entry = readdir(dir))) {
            if(entry -> d_name[0] != '.' && (arg[i+1] == NULL || entry -> d_type == DT_DIR || entry -> d_type == DT_LNK) && endWith(entry -> d_name, token+1)){
                char * s = malloc(strlen(chemin) + strlen(entry -> d_name) + 2);
                if(strlen(chemin) > 0)
                    strcpy(s, chemin);
                else
                    s[0] = '\0';
                strcat(s, entry -> d_name);
                etoile(s, arg, tab, i+1);
            }
        }
        closedir(dir);
        free(chemin);
    }
}

/* prend un mot de la forme "a/b/aa et crée un tableau de la forme ["a";"b";"aa";"bb"] */
void tabtok(char * t, char * a[]){
    if(t[0] == '/')
        ajouter("/", a);

    t = strtok(t, "/");

    while(t != NULL){
        if(t[0] == '*' && t[1] == '*'){
            ajouter("*", a);
            ajouter("*", a);
        }else{
            ajouter(t, a);
        }
        t = strtok(NULL, "/");
    }
}

/* interpréteur des lignes de commandes */
int interpreteur(char *ligne) {
    char copy[BUFFER_SIZE];
    strcpy(copy, ligne);

    if(ligne == NULL)
        return 127;

    /* cas des tubes nommés */
    if(strstr(copy, " < ")){
      char * cmd1 = strtok(copy, "<");
      char * cmd2 = strtok(NULL, "<");
      return tube_nomme(cmd1, cmd2, "<");
    }
    if(strstr(copy, " > ")){
      char * cmd1 = strtok(copy, ">");
      char * cmd2 = strtok(NULL, ">");
      return tube_nomme(cmd1, cmd2, ">");
    }
    if(strstr(copy, " >| ")){
      char * cmd1 = strtok(copy, ">|");
      char * cmd2 = strtok(NULL, ">|");
      return tube_nomme(cmd1, cmd2, ">|");
    }
    if(strstr(copy, " >> ")){
      char * cmd1 = strtok(copy, ">>");
      char * cmd2 = strtok(NULL, ">>");
      return tube_nomme(cmd1, cmd2, ">>");
    }
    if(strstr(copy, " 2> ")){
      char * cmd1 = strtok(copy, "2>");
      char * cmd2 = strtok(NULL, "2>");
      return tube_nomme(cmd1, cmd2, "2>");
    }
    if(strstr(copy, " 2>| ")){
      char * cmd1 = strtok(copy, "2>|");
      char * cmd2 = strtok(NULL, "2>|");
      return tube_nomme(cmd1, cmd2, "2>|");
    }
    if(strstr(copy, " 2>> ")){
      char * cmd1 = strtok(copy, "2>>");
      char * cmd2 = strtok(NULL, "2>>");
      return tube_nomme(cmd1, cmd2, "2>>");
    }

    /* cas du tube anonyme */
    if(strchr(copy, '|')!=NULL){
      return tube_anonyme(copy);
    }

    if(!strcmp(ligne,""))
        return val_retour;
    
    char * argv[BUFFER_SIZE];
    char * token;
    token = strtok(ligne, " ");
    int c = 0;

    /* on place les éléments de la commande dans un tableau */
    while(token!=NULL) {
      
        argv[c++] = token;
        token = strtok(NULL, " ");
    }
    
    argv[c]=NULL;
    
    if(c>=MAX_ARGS_NUMBER){
      return 1;
    }

    /* cas où il y a trop d'arguments */
    for(int i=0;i<c;i++){
      if(strlen(argv[i])>=MAX_ARGS_STRLEN){
        return 1;
      }
    }
    
    char * a[BUFFER_SIZE];
    a[0] = NULL;
    
    /* modification du tableau contenant la ligne de commande si des jokers sont présents */
    for(int i = 0; i < c ; i++){
        char * b[BUFFER_SIZE];
        b[0] = NULL;

        char * s = argv[i];
        char res[strlen(s)];
        int n = 0;
        for(int i = 0 ; i<strlen(s) ; i++){
            
            /* gestion du cas où il y a deux slash à la suite dans un chemin on en ajoutant qu'un */
            if(i != 0 && (s[i] == '/' && s[i-1] == '/')) {

            } else {
                res[n++] = s[i];
            }
            
        }
        
        res[n] = '\0';

        tabtok(res, b);
        etoile(NULL, b, a, 0);
    }

    /* renvoie vers la gestion des commandes internes/externes */
    int x = traitement_commande(copy,a);
    
    /* libère les éléments du tableau */
    int i = 0;
    while(a[i] != NULL)
        free(a[i++]);
    return x;
}

/* construction du prompt */
void creer_prompt(char *tmp, char *couleur, char *buffer) {
    
    /* premier prompt affiché au début de l'éxécution de slash*/
    if (val_retour == 255) 
        sprintf(tmp,"%s[SIG]\033[36m%s%s",couleur,buffer,"\033[00m$ ");
    /* les autres prompt avec la valeur de retour présente */
    else
        sprintf(tmp,"%s[%d]\033[36m%s%s",couleur,val_retour,buffer,"\033[00m$ ");

}

/* raccourcit le prompt pour atteindre les 30 caractères */
void raccourcir_prompt(char *aux, char *buffer, int n) {
    
    int b = 30 - (7 + n) ;
    int a = strlen(buffer) - b;
    /* les caractères en trop sont cachés par "..." */
    sprintf(aux,"%s%s","...",buffer+a);
    
}

/* renvoie le nombre de chiffres contenus dans la valeur de retour qu'on affiche au début du prompt */
int nbr_chiffres (int n) {
    int i = 1;
    while (n >= 10) {
        n /= 10;
        i++;
    }

    return i;
}

/* construction du prompt */
void prompt(char *buffer, char *tmp, char *couleur) {
    
    int n = nbr_chiffres(val_retour);
    
    /* on vérifie que la taille du prompt est conforme, si ce n'est pas le cas on le raccourcit */
    if (strlen(buffer) > (23 - n) + 1) {
        char aux[BUFFER_SIZE];
        raccourcir_prompt(aux, buffer, n);
        creer_prompt(tmp, couleur, aux);
    } else {
        creer_prompt(tmp, couleur, buffer);
    }
}

/* gestion des signals */
void signal_detecte(int sig){
    if(sig == SIGSEGV)
        exit(val_retour);
    val_retour = 255;
}

int main(){

    /* WARNING -> "signal" est obsolète, tentative de remplacement par sigaction */
    for(int i = 0; i < 64 ; i++)
        signal(i, &signal_detecte);

    
   /* for (int i = 1; i < 64; i++) {
        
        struct sigaction sa = {0};
        
        switch(i) {
                
            case SIGINT :
            case SIGTERM : sa.sa_handler = SIG_IGN; break;
            default : sa.sa_handler = signal_detecte;
                
        }
        
        sigaction(i,&sa,NULL);
    }*/
    

    rl_outstream = stderr;

    while (1) {
        char a[BUFFER_SIZE];
        
        /* met à jour la couleur du prompt en fonction de la valeur de retour */
        prompt(getenv("PWD"), a, (val_retour) ? ROUGE : VERT);

        /* récupère la nouvelle commande */
        char * ligne = readline(a);
        /* l'ajoute à l'historique*/
        add_history(ligne);
        /* met à jour la valeur de retour */
        val_retour = interpreteur(ligne);

        free(ligne);
    }
    
    return 0;
}
