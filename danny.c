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
void processConfig(Data* data, const char* file){
    char buffer[128];
    char ptr;
    int fd = -1;
    int count = 0;

    fd = open(file, O_RDONLY);
    if (fd < 0) {
      sprintf(buffer, "Error opening %s", file);
      perror(buffer);
      raise(SIGINT);
    }
    else{
       write(1, "No Error\n", 9);
       
       //Memory for name
        data->station = (char*) malloc(sizeof(char));
        count++;
        read(fd, &ptr, 1);
        while(ptr != '\n'){
            data->station[count-1] = ptr;
            data->station = realloc(data->station, (++count)*sizeof(char));
            read(fd, &ptr, 1);
        }
        data->station[count - 2] = '\0';

        //Memory for path
        count = 0;
        data->path = (char*) malloc(sizeof(char));
        count++;
        read(fd, &ptr, 1);
        while(ptr != '\n'){
            data->path[count-1] = ptr;
            data->path = realloc(data->path, (++count)*sizeof(char));
            read(fd, &ptr, 1);
        }
        data->path[count - 2] = '\0';

        //Getting time
        count = 0;
        read(fd, &ptr, 1);
        while(ptr != '\n'){
            buffer[count++] = ptr;
            read(fd, &ptr, 1);
        }
        buffer[count] = '\0';
        data->time = atoi(buffer);
        
        //Memory for Jack server IP
        count = 0;
        data->jackIP = (char*) malloc(sizeof(char));
        count++;
        read(fd, &ptr, 1);
        while(ptr != '\n'){
            data->jackIP[count-1] = ptr;
            data->jackIP = realloc(data->jackIP, (++count)*sizeof(char));
            read(fd, &ptr, 1);
        }
        data->jackIP[count - 2] = '\0';

        //Memory for Jack port
        count = 0;
        data->jackPort = (char*) malloc(sizeof(char));
        count++;
        read(fd, &ptr, 1);
        while(ptr != '\n'){
            data->jackPort[count-1] = ptr;
            data->jackPort = realloc(data->jackPort, (++count)*sizeof(char));
            read(fd, &ptr, 1);
        }
        data->jackPort[count - 2] = '\0';

        //Memory for Wendy server IP
        count = 0;
        data->wendyIP = (char*) malloc(sizeof(char));
        count++;
        read(fd, &ptr, 1);
        while(ptr != '\n'){
            data->wendyIP[count-1] = ptr;
            data->wendyIP = realloc(data->wendyIP, (++count)*sizeof(char));
            read(fd, &ptr, 1);
        }
        data->wendyIP[count - 2] = '\0';

        //Memory for Wendy port
        count = 0;
        data->wendyPort = (char*) malloc(sizeof(char));
        count++;
        read(fd, &ptr, 1);
        while(ptr != '\n'){
            data->wendyPort[count-1] = ptr;
            data->wendyPort = realloc(data->wendyPort, (++count)*sizeof(char));
            read(fd, &ptr, 1);
        }
        data->wendyPort[count - 2] = '\0';


        //Close file
        close(fd);

        sprintf(buffer, "Station name is -%s-\n", data->station);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "Path name is -%s-\n", data->path);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "Time is -%d-\n", data->time);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "JackIP is -%s-\n", data->jackIP);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "JackPort is -%s-\n", data->jackPort);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "WendyIP is -%s-\n", data->wendyIP);
        write(1, buffer, strlen(buffer));
        sprintf(buffer, "WendyPort is -%s-\n", data->wendyPort);
        write(1, buffer, strlen(buffer));
    }
}

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
    Data data;

    //Reprogram signals
    for (size_t i = 0; i < 64; i++) signal(i, ksighandler);

    //Process configuration file
    write(1, EOL, sizeof(EOL));
    write(1, STARTING, sizeof(STARTING));
    write(1, EOL, sizeof(EOL));
    processConfig(&data, argv[argc -1]);


    /*while (1)
    {
        //Listen to folder
        pause();
    }*/


    return 0;
}
