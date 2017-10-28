CC = gcc -ggdb -std=c99 -Wall -Werror -O3
OUT = read_random

all:
	$(CC) -o $(OUT) read_random.c

clean:
	rm -f *.o
	rm -f $(OUT)
	rm -f random_ints.bin

run: clean all
	./$(OUT)

rerun: all
	./$(OUT)

valgrind: clean all
	valgrind -v --leak-check=full ./$(OUT)