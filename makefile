danny: danny.c
	gcc -g -Wall -Wextra -o danny danny.c

.PHONY: clean
clean:
	rm -rf *.o
