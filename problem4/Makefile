CC = mpixlc
CFLAGS = -Wall -Werror -fsanitize=address

all: problem4

problem4: problem4.c
	$(CC) $(CFLAGS) $< -o $@

test: test.sh
	sh test.sh

clean:
	rm -f *.o problem4