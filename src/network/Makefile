
CC = gcc
CCFLAGS = -Wall -Wextra


all: sockslist.o

clean:
	rm -rf *.o

clobber: clean
	rm -rf *~

test: all
	cd unit_test && make

cleanTest:
	cd unit_test && make clean

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@
