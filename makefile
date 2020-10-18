CC=gcc
CFLAGS= -g -lm -Wall -Wextra
DEPS = connectionManager.h functions.h libraries.h 
OBJ = danny.o connectionManager.o functions.o

%.o: %.c $(DEPS)
	@echo "Compiling project"
	$(CC) -c -o $@ $< $(CFLAGS)

danny: $(OBJ)
	@echo "Working on danny"
	$(CC) -o $@ $^ $(CFLAGS)
	@echo "Done!"

.PHONY: clean

clean:
	@echo "Cleaning up"
	rm -f *.o