CC = gcc
CCFLAGS = -Wall -g

all: test_sockslist

clean:
	rm -f *.out

test_%: test_%.c
	$(CC) $(CCFLAGS) $< ../*.o -o $@.out
	./$@.out
