/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "connectionManager.h"
#include "functions.h"

//FUNCTIONS
void fill(char** target, int fd, char delimiter){
    char ptr;
    int count = 0;

    *target = (char*) malloc(sizeof(char));
    count++;
    read(fd, &ptr, 1);
    while(ptr != delimiter){
        *target[count-1] = ptr;
        printf("%c = %c\n", *target[count-1], ptr);
        print("1\n");
        *target = realloc(*target, (++count)*sizeof(char));
        printf("%s\n", *target);
        read(fd, &ptr, 1);
        printf("%d, %d\n", ptr, delimiter);
    }
    *target[count - 2] = '\0';
    printf("-%s-\n", *target);
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
        //fill(&data->station, fd, '\n');

        count = 0;
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

        
        /*sprintf(buffer, "Station name is -%s-\n", data->station);
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
        write(1, buffer, strlen(buffer));*/
        
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
