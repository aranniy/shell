CC=gcc
CFLAGS= -g -Wall
LDLIBS= -lreadline

ALL= slash
all: $(ALL)

slash.o : slash.c
cmd_internes.o : cmd_internes.c cmd_internes.h slash.o
cmd_externes.o : cmd_externes.c cmd_externes.h slash.o
redirections.o : redirections.c redirections.h slash.o
slash : slash.o cmd_internes.o cmd_externes.o redirections.o

clean:
	rm  -rf *~  *.o  slash
