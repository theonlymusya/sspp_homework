CC = gcc
CFLAGS = -pthread -Wall -Werror -fsanitize=address

all: problem0

problem0: problem0.c
	$(CC) $(CFLAGS) $< -o $@

test: test.sh
	sh test.sh

clean:
	rm -f *.o problem0
