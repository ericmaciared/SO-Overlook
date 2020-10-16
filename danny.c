/**
 * Authors: Eric Macià Redono (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/

//LIBRARIES

#include <fcntl.h> 
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>


//MODULES


//DEFINES

#define EOL "\n"
#define STARTING "Starting Danny...\n"
#define DISCONNECTING "Disconnecting Danny...\n"

//STRUCTS
typedef struct Data{
  char* station;
  char* path;
  int time;
  char* jackIP;
  char* jackPort;
  char* wendyIP;
  char* wendyPort;
}Data;

//FUNCTIONS

void ksighandler(int signum){
    switch (signum)
    {
    case SIGINT:
        write(1, EOL, sizeof(EOL));
        write(1, DISCONNECTING, sizeof(DISCONNECTING));
        write(1, EOL, sizeof(EOL));
        //TODO: Free memory function
        exit(0);
        break;

    default:
        break;
    }
    signal(signum, ksighandler);
}


int main(int argc, char const *argv[])
{
    char buffer[128];
    char ptr;

    //Data file processing
    Data data;
    int fd = -1;
    int count = 0;

    //Reprogram signals
    for (size_t i = 0; i < 64; i++) signal(i, ksighandler);

    //Process configuration file
    sprintf(buffer, "Opening file -%s-\n", argv[argc - 1]);
    write(1, buffer, strlen(buffer));

    write(1, EOL, sizeof(EOL));
    write(1, STARTING, sizeof(STARTING));
    write(1, EOL, sizeof(EOL));

    fd = open(argv[argc-1], O_RDONLY);
    if (fd < 0) {
      sprintf(buffer, "Error opening %s", argv[argc-1]);
      perror(buffer);
      raise(SIGINT);
    }
    else{
       write(1, "No Error\n", 9);
       
       //Memory for name
        data.station = (char*) malloc(sizeof(char));
        count++;
        read(fd, &ptr, 1);
        while(ptr != '\n'){
            data.station[count-1] = ptr;
            data.station = realloc(data.station, (++count)*sizeof(char));
            read(fd, &ptr, 1);
        }
        data.station[count - 2] = '\0';

        //Memory for 


        sprintf(buffer, "Station name is -%s-\n", data.station);
        write(1, buffer, strlen(buffer));
        

    }




    while (1)
    {
        //Listen to folder
    }


    return 0;
}
