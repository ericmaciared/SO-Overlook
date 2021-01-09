CC=gcc
CFLAGS= -g -lm -Wall -Wextra
DEPS = dannyManager.h protocolManager.h functions.h libraries.h 
OBJ = danny.o dannyManager.o protocolManager.o functions.o

%.o: %.c $(DEPS)
	@echo "Compiling project"
	$(CC) -c -o $@ $< $(CFLAGS)

danny: $(OBJ)
	@echo "Working on danny"
	$(CC) -o $@ $^ $(CFLAGS)
	rm *.o
	@echo "Done!"

.PHONY: clean

clean:
	@echo "Cleaning up"
	rm *.o
