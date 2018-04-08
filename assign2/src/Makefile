PRG=VMsimulator
SRC=main.c pageTable.c loadPages.c
OBJ=$(SRC:.c=.o)
CC=gcc
RM=rm -f
INCLPATH=-I../include
LDLIBS=-lm
LDFLAGS+=

CFLAGS+= $(INCLPATH)
CPPFLAGS+= -pedantic-errors -Wall -Werror --pedantic -std=c99 


all: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $(PRG) -g
clean:
	$(RM) *.o

fclean: clean
	$(RM) $(PRG)

re: fclean all
