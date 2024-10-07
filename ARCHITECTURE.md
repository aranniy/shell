Stratégie adoptée :

    Pour le semestre 5, nous avons reçu la tâche de programmer par groupe de trois étudiants un interpréteur de commandes intéractif. Nous allons établir ici la stratégie adoptée afin de mener à bien ce projet avec notamment la description de l'architecture logicielle, la structure de données ainsi que les algorithmes utilisés. 
    
    Commandes internes : 
    
        Grâce à l'implémentation du système de "jalon", nous avons pu assez facilement mettre en place un ordre dans l'implémentation des fonctionnalités de notre shell "slash". Nous avons ainsi commencé par la réalisation des commandes internes "exit", "cd" et "pwd". Pour cela, nous avons commencé par créer une fonction interpréteur qui va récupérer le premier argument de la ligne de commande tapée par l'utilisateur et qui, selon sa valeur va envoyer la commande se faire traiter (via un switch) par une fonction spécifique adaptée à chaque commande. Il a également fallu rajouter une fonction faisant la différence entre les possibles options des commandes pour faire le traitement adapté. Afin de faciliter ce traitement, nous avons décidé de conserver tous les arguments de la commande dans un tableau pour pouvoir y accéder rapidement et à tout moment. 
        
        PWD : fonction "cmd_pwd"
        
            -L | "default" : Pour afficher la référence logique, nous avons récupéré la valeur de la variable d'environnement "PWD". 
            
            -P : Pour afficher la référence physique, nous avons utilisé la fonction getcwd qui prend un buffer et sa taille en argument pour stocker le chemin courant dans le buffer.
        
        EXIT : fonction "cmd_exit"
        
            Cette commande a été assez facile à implémenter. Si l'utilisateur propose une valeur de retour on vérifie qu'elle est conforme, et on utilise la fonction "exit" avec cette valeur si oui. Si l'utilisateur n'en a pas fourni, on décide de faire un exit avec la valeur de retour actuelle. 
        
        CD : fonction "cmd_cd"
        
            Contrairement aux deux autres commandes internes, l'implémentation de cd a nécéssité l'implémentation de plus d'une fonction en raison de ses nombreuses options : 
            
            -L | "default" : fonction "cd_L"
            
                On essaye d'abord de faire le changement de chemin de façon logique. On ajoute d'abord le chemin ref donné par l'utilisateur à la fin du chemin courant (sauf si ref est un chemin absolu, dans ce cas là il remplace le chemin courant) puis on appelle chdir sur le nouveau chemin formé (et stocké dans la variable d'environnement "PWD"). En cas d'echec, on tente plutôt l'approche physique (on appelle chdir sur ref directement). En cas d'échec les variables d'environnements sont restaurés à leur valeur d'origine, sinon on les met à jour à nouveau.
            
            -P : fonction "cd_P"
            
                On appelle chdir sur le chemin passé en paramètre, en cas de succès on met à jour les variables d'environnements "OLDPWD" et "PWD".
            
            "home" : fonction "cd_home"
            
                En l'absence de paramètres, on appelle chdir sur la valeur de la variable d'environnement "HOME", puis on met à jour la valeur de "PWD" et "OLDPWD".
            
            "tiret" : fonction "cd_tiret"
            
                On appelle chdir sur la valeur de la variable d'environnnement "OLDPWD" puis on interverti les valeurs de "PWD" et "OLDPWD".
    
    Conformité du prompt : 

        Valeur de retour : géré dans le main
            
            Nous devions afficher la valeur de retour de la commande précédente au début de notre prompt. Pour cela, nous avons créé une variable globale qui est mise à jour après chaque commande tapée en fonction de succès ou de l'échec des fonctions appelées par l'interpréteur. Si une fonction ne fonctionne pas car la commande est erroné, on renvoie un. Si la commande a été traité avec succès, on renvoie 0.
            
        Référence logique du répertoire courant : géré dans le main
        
            Il nous a suffit de récupérer la réference du répertoire courant via la variable d'environnement "PWD".
            
        Bascule de couleur : géré dans le main
        
            En fonction de la valeur de retour nous avons rendu son affichage dans le prompt rouge (val_retour = 0) ou vert (val_retour = 1), à l'aide de codes de bascule de couleurs. La référence du répertoire courant est affichée en bleu. Nous avons également ajouté des balises de couleurs pour éviter les problèmes d'affichages.
        
        Limite de 30 caractères : fonction "raccourcir_prompt" + "prompt"
            
            Puisque le prompt doit faire 30 caractères il faut que la référence du répertoire courant ait une taille maximale, en ne comptant pas le nombre de chiffres dans la valeur de retour, les deux crochets qui l'entourent, le $ et l'espace juste après. Si la taille dépasse, on retire les caractères en trop et on ajoute "..." au début à l'aide de la fonction "raccourcir_prompt".
        
        Construction du prompt : fonction "creer_prompt"
        
            Nous avons créé une fonction creer_prompt qui prend en paramètre un chemin, une couleur et la valeur de retour et qui crée le prompt grâce à "sprintf". (exemple d'utilisation de sprintf :  sprintf(valeur_retour,"%s[SIG]\033[36m%s%s",couleur,chemin,"\033[00m$ ");).
            
        Une boucle while est présente dans le main de notre fichier slash et effectue 4 actions :
            - création du prompt 
            - récupération de la ligne de commande tapée par l'utilisateur (grâce à readline)
            - ajout de la ligne dans l'historique de readline
            - mise à jour de la valeur de retour
        
        Signals : géré dans le main + fonction "signal_detecté"
    
        Le shell "slash" étant censé ignorer les signaux SIGINT et SIGTERM, nous avons voulu implémenter le type struct sigaction et affecter à ces signaux le flag SIG_IGN
        afin qu'ils soient ignorés, et pour les autres une fonction "signal_detecté" qui change alors la valeur de retour par "255".
        Toutefois, nous avons eu des problèmes lors des test avec cette méthode. Nous nous donc dû nous rabattre sur notre solution précédente consistant à utiliser la fonction obsolète "signal". Nous sommes au courant que cette fonction n'est pas dite de confiance, et qu'elle doit être remplacée.
        
    Commandes externes : 

	Les commandes externes sont gerées par la fonction cmd_externe. Il nous suffit pour ça de créer de nouveaux processus avec un fork, dont le fils sera remplacé par execvp qui executera la commande externe avec ses arguments si elle en a. Le père renverra la valeur de retour du fils après l'avoir attendu.
    
    Jokers : 
    
        Joker simple :
        
            Nous avons implémenté une fonction "étoile" qui prend en argument un tableau qui contient les arguments tapés par l'utilisateur en ligne de commande 
            et qui renvoie un tableau contennant les arguments correspondants. 
            Donc si une étoile est trouvé dans un des arguments, il sera supprimé et seront ajoutés au tableau tous les éléments de la forme de l'argument initial. 
            Ainsi, nous utilisons ce tableau pour exécuter les commandes externes.
        
        Joker double :
        
            Pour le joker double, nous utlisons la fonction "étoile" de la même manière que pour le joker simple, sauf que nous transformons "**" en "*/*" avant l'appel de la fonction.
        
    Redirections : 

	Fonction tube_anonyme : Nous devions d'abord gerer les cas des redirections sur un tube anonyme, sachant que les redirections additionnelles sur un même pipeline sont autorisées. Pour cela, nous séparons déjà chaque commande entre "|", que nous divisons aussi par espace pour en séparer leurs arguments possibles. Il suffit ensuite de garder la trace du pipe précédent dans une boucle pour exécuter chaque commande. Cela dit, un problème persiste toutefois : Une fois la commande exécutée, le prompt est quitté.
	Fonction tube_nomme : Pour la redirection d'une commande vers un fichier, il faut que le fichier soit ouvert avec les indications qui correspondent à la redirection voulue. Selon le cas, on transforme l'entrée standard, la sortie standard ou la sortie erreur standard en une copie du descripteur de fichier. Après tout ça, on exécute la commande de la même façon dont on procèderait pour les fonctions externes.
        
    Architecture logicielle : 
        
        Nous avons décidé de répartir notre projet en plusieurs fichiers :
        - slash.c qui contient le main ainsi que l'interpréteur de commandes 
        - cmd_internes.c qui contient les fonctions servant aux commandes internes
        - cmd_externes.c qui contient les fonctions servant aux commandes externes
        - redirections.c qui contient les fonctions liées à la redirection de commandes 
    
    Structure de données :
    
        - variable globale "val_retour" permettant de stocker la valeur de retour
        - tableau permettant de stocker les différents arguments d'une commande
        - tableau de tableaux pour stocker les élements de chaque commande séparé par des pipeline par exemple
        - plusieurs tableaux intermédiaires facilitant le remplacement des arguments contenant des jokers 
        - un char * ligne représentant la commande
        - plusieurs buffers permettant de stocker la référence d'un chemin
        - présence de descripteurs 
