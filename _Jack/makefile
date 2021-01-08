CC=gcc
CFLAGS= -g -Wall -Wextra -lpthread
<<<<<<< Updated upstream
DEPS = jackManager.h lloydManager.hprotocolManager.h functions.h libraries.h
=======
<<<<<<< Updated upstream
DEPS = jackManager.h lloydManager.hprotocolManager.h functions.h libraries.h
=======
DEPS = jackManager.h lloydManager.hprotocolManager.h functions.h libraries.h semaphore_v2.h
>>>>>>> Stashed changes
>>>>>>> Stashed changes
OBJ = jack.o jackManager.o lloydManager.o protocolManager.o functions.o

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
