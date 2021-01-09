CC=gcc
CFLAGS= -g -Wall -Wextra -lpthread
DEPS = wendyManager.h protocolManager.h functions.h libraries.h 
OBJ = wendy.o wendyManager.o protocolManager.o functions.o

%.o: %.c $(DEPS)
	@echo "Compiling project"
	$(CC) -c -o $@ $< $(CFLAGS)

wendy: $(OBJ)
	@echo "Working on wendy"
	$(CC) -o $@ $^ $(CFLAGS)
	rm *.o
	@echo "Done!"

.PHONY: clean

clean:
	@echo "Cleaning up"
	rm *.o
