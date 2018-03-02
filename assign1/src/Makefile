PRG=assign1
SRC=main.c fib.c producer.c consumer.c time.c queue.c
OBJ=$(SRC:.c=.o)
CC=gcc
RM=rm -f
INCLPATH=-I../include
LIBPATH=
LDLIBS=-lpthread
LDFLAGS+=

CFLAGS+= $(INCLPATH)
CPPFLAGS+= -pedantic-errors -Wall


all: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $(PRG)


clean:
	$(RM) *.o

fclean: clean
	$(RM) $(PRG)

re: fclean all
