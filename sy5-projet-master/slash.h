#ifndef slash_h
#define slash_h
#include "cmd_internes.h"
#include "cmd_externes.h"
#include "redirections.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/dir.h>
#include <limits.h>
#include <errno.h>

#define BUFFER_SIZE 4096

#define VERT "\033[32m"
#define ROUGE "\033[31m"
#define CYAN "\033[36m"
#define NORMAL "\033[00m"

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

#endif /* slash_h */
