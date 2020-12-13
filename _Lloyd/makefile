CC=gcc
CFLAGS= -g -Wall -Wextra -lpthread
DEPS = lloydManager.h functions.h libraries.h
OBJ = lloyd.o lloydManager.o functions.o

%.o: %.c $(DEPS)
	@echo "Compiling project"
	$(CC) -c -o $@ $< $(CFLAGS)

lloyd: $(OBJ)
	@echo "Working on lloyd"
	$(CC) -o $@ $^ $(CFLAGS)
	@echo "Done!"

.PHONY: clean

clean:
	@echo "Cleaning up"
	rm *.o
