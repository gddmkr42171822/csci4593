CC=gcc
CFLAGS= -g -Wall -Wextra -std='c99'
RM= rm -f

all: memsim

# $@ matches the target; $< matches the first dependent
memsim: main.o memsim.o
	$(CC) -o memsim main.o memsim.o -lm

main.o: main.c memsim.h
	$(CC) $(CFLAGS) -c $<

memsim.o: memsim.c memsim.h
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) *.o memsim *.txt
