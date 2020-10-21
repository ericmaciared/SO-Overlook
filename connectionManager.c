/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "connectionManager.h"
#include "functions.h"

//FUNCTIONS
char* fill(int fd, char delimiter){
    char ptr;
    char* target = NULL;
    int count = 0;

    target = (char*) malloc(sizeof(char));
    count++;
    read(fd, &ptr, 1);
    while(ptr != delimiter){
        target[count-1] = ptr;
        target = realloc(target, (++count)*sizeof(char));
        read(fd, &ptr, 1);
    }
    target[count - 2] = '\0';
    return target;
}

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
        //Memory for name
        data->station = fill(fd, '\n');

        //Memory for path
        data->path = fill(fd, '\n');

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
        data->jackIP = fill(fd, '\n');

        //Memory for Jack port
        data->jackPort = fill(fd, '\n');

        //Memory for Wendy server IP
        data->wendyIP = fill(fd, '\n');

        //Memory for Wendy port
        data->wendyPort = fill(fd, '\n');

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

void freeConfig(Data* data){
    free(data->station);
    free(data->path);
    free(data->jackIP);
    free(data->jackPort);
    free(data->wendyIP);
    free(data->wendyPort);

    data->station = NULL;
    data->path = NULL;
    data->jackIP = NULL;
    data->jackPort = NULL;
    data->wendyIP = NULL;
    data->wendyPort = NULL;
}
