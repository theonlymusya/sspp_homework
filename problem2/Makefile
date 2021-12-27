CC = gcc
CFLAGS = -pthread -Wall -Werror -fsanitize=address

all: int_problem2 long_long_problem2

int_problem2: problem2.c
	$(CC) $(CFLAGS) $< -o $@

long_long_problem2: problem2.c
	$(CC) $(CFLAGS) $< -o $@

test: test.sh
	sh test.sh

clean:
	rm -f *.o int_problem2 long_long_problem2