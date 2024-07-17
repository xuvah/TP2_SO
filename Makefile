CFLAGS= -lpthread -lm -Wall

SAIDA=saidaTP*

CC:=gcc
#--------------- Nome da saida ---------------
EXEC:=main
#----------------- Magica --------------------
RM:=rm -rf

HRD:=$(wildcard *.h)
SRC:=$(wildcard *.c)
OBJ:=$(SRC:.c	=.o)

#---------------- Execucao -------------------

all:$(EXEC)

$(EXEC): $(OBJ) $(HDR)
	$(CC) $^ -g -o $(EXEC) $(CFLAGS)

%.o:  %.c $(HDR)
	$(CC) $^ -c $(CFLAGS)

limpar:
	clear

dbug:
	$(CC) $(SRC) -o $(EXEC) $(CFLAGS) -g

clean:
	$(RM) $(OBJ) $(EXEC) $(SAIDA)
