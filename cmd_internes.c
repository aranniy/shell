#include "cmd_internes.h"


/* source : https://www.interviewbit.com/problems/simplify-directory-path/
 * permet de simplifier un chemin en s'occupant des cas "." et ".."
 */
char* simplifier_chemin(char* A) {
    int n = strlen(A);
    char *s = malloc(n+1);
    int i = 0;
    int t = 0;
    /* on rajoute un slash au début du chemin */
    s[0] = '/';
    for(i = 0;i<n;i++)
    {
        /* cas ".." -> on retire le répertoire précedant du chemin */
        if(i<n-1 && A[i]=='.' && A[i+1]=='.')
        {
            i+=2;
            if(t!= 0 && s[t]=='/') t--;
            while(s[t]!='/') t--;
        }
        /* on vérifie que le caractère "." représente le répertoire courant et n'est pas contenu dans un nom de répertoire/fichier */
        else if(i<n && A[i]=='.' && (i>0 && (A[i-1]== '/' || A[i+1] == '/')))
        {
            i++;
        }
        else if(A[i]=='/' && s[t]=='/') ;
        else s[++t]= A[i];
    }
    /* si le dernier caractère du chemin est un slash, on le retire puis on ajoute le caractère nul */
    if(s[t]=='/' && t!=0) t--;
    s[++t] = '\0';
    return s;
}

/* met à jour le chemin */
void ajout(char *ref) {

    char * chemin = malloc(strlen(ref) + 2 + strlen(getenv("PWD")));
    if(chemin == NULL) abort();
    sprintf(chemin,"%s/%s",getenv("PWD"),ref);
    char * a = simplifier_chemin(chemin);
    /* met à jour le path PWD */
    setenv("PWD",a,1);
    free(chemin);
    free(a);
}

/* commande cd avec l'option L (défault) */
int cd_L(char * ref) {
    
    /* on conserve les chemins de PWD et OLDPWD au cas où le nouveau chemin est invalide */
    char * chemin = malloc(strlen(getenv("PWD")) + 1);
    char * chemin2 = malloc(strlen(getenv("OLDPWD")) + 1);
    strcpy(chemin,getenv("PWD"));
    strcpy(chemin2,getenv("OLDPWD"));
    
    /* si le chemin est absolu, on doit mettre à jour le path PWD à "" pour que la fonction ajout puisse partir de zéro */
    if (ref[0]=='/') {
        setenv("OLDPWD",getenv("PWD"),1);
        setenv("PWD","",1);
        ajout(ref);
    } else {
        ajout(ref);
    }
    
    setenv("OLDPWD",chemin,1);

    /* interprétation logique tout d'abord */
    if (chdir(getenv("PWD")) < 0) {
        
        /* interprétation physique sinon */
        if (chdir(ref) < 0) {
            /* traitement physique */
            setenv("OLDPWD",chemin2,1);
            setenv("PWD",chemin,1);
            free(chemin);
            free(chemin2);
            return 1;
        }
        
        char buffer[BUFFER_SIZE];
        if (getcwd(buffer,BUFFER_SIZE) == NULL)
            return 1;
        
        setenv("PWD",buffer,1);
        
    }
    
    free(chemin);
    free(chemin2);
    
    return 0;
    
}

/* commande cd avec l'option P -> traitement physique */
int cd_P(char * ref){

    /* on tente changer le repertoire courant par ref, erreur sinon */
    if(chdir(ref) < 0){
        printf("cd: no such file or directory: %s\n", ref);
        return 1;
    }
    
    /* on met à jour les variables d'environnements OLDPWD et PWD */
    setenv("OLDPWD",getenv("PWD"),1);
    char buffer[BUFFER_SIZE];
    if (getcwd(buffer,BUFFER_SIZE) == NULL) 
        return 1;
    setenv("PWD",buffer,1);

    return 0;
}

/* commande cd seul */
int cd_home() {
    
    if (chdir(getenv("HOME")) < 0) 
        return 1;
    
    /* on met à jour le path PWD en lui transmettant celle de HOME, et on conserve le chemin courant dans OLDPWD */
    setenv("OLDPWD",getenv("PWD"),1);
    setenv("PWD",getenv("HOME"),1);

    return 0;
}

/* commande cd avec un tiret */
int cd_tiret() {
    
    if (chdir(getenv("OLDPWD")) < 0) return 1;

    /* permet de revenir au chemin précédent */
    char * chemin = malloc(strlen(getenv("PWD")) + 1);
    strcpy(chemin,getenv("PWD"));
    setenv("PWD",getenv("OLDPWD"),1);
    setenv("OLDPWD",chemin,1);
    free(chemin);
    return 0;
}

/* traitement de la commande cd en fonction de ses options */
int cmd_cd(char * token){

    if (token == NULL)
        return cd_home();

    if (!strcmp(token,"-"))
        return cd_tiret();

    if(!strcmp(token,"-P"))
        return cd_P(strtok(NULL," "));

    if(!strcmp(token,"-L"))
        token = strtok(NULL," ");

    return cd_L(token);
}

/* commande pwd  */
int cmd_pwd(char * token){
    
    /* traitement en fonction de ses options */
    
    /* l'option -L par défaut -> affiche la référence absolu physique */
    if (token == NULL || (!strcmp(token,"-L") && strtok(NULL, " ") == NULL)){
        
        /* on utilise la variable environnemental PWD */
        printf("%s\n",getenv("PWD"));
        return 0;
        
    /* l'option -P -> affiche la référence absolu logique */
    } else if (!strcmp(token,"-P") && strtok(NULL, " ") == NULL){
        char buffer[BUFFER_SIZE];
        
        /* on utilise la fonction getcwd */
        if (getcwd(buffer,BUFFER_SIZE) == NULL)
            return 1;
        
        printf("%s\n",buffer);
        return 0;
    }
    
    /* erreur */
    printf("pwd: bad option or too many arguments: %s\n", token);
    return 1;
    
}
