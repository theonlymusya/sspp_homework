CC = clang++
CFLAGS = -Wall -Werror -fsanitize=address

all: problem1

problem1: problem1.cpp
	$(CC) $(CFLAGS) $< -o $@

test: test.sh
	sh test.sh

clean:
	rm -f *.o problem1
