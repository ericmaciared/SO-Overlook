CC=gcc
CFLAGS= -g -Wall -Wextra -lpthread
DEPS = jackManager.h protocolManager.h functions.h libraries.h 
OBJ = jack.o jackManager.o protocolManager.o functions.o

%.o: %.c $(DEPS)
	@echo "Compiling project"
	$(CC) -c -o $@ $< $(CFLAGS)

jack: $(OBJ)
	@echo "Working on jack"
	$(CC) -o $@ $^ $(CFLAGS)
	rm *.o
	@echo "Done!"

.PHONY: clean

clean:
	@echo "Cleaning up"
	rm *.o
