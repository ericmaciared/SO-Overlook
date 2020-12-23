/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "dannyManager.h"

//PRIVATE FUNCTIONS
int checkExtension(char* filename){
    int code = NOFILE;
    char end[4];

    if (strlen(filename) < 3) return code;

    end[0] = filename[strlen(filename) - 3];
    end[1] = filename[strlen(filename) - 2];
    end[2] = filename[strlen(filename) - 1];
    end[3] = '\0';

    if (!strcmp(end, "txt")) code = TXT;
    if (!strcmp(end, "jpg")) code = JPG;

    return code;
}

void getStationData(char* filename, StationData* data){
    char buffer[32];
    int fd = -1;

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
      sprintf(buffer, "Error opening %s", filename);
      perror(buffer);
      exit(EXIT_FAILURE);
    }
    else {
        data->dateString = readUntil(fd,'\n');
        data->hourString = readUntil(fd,'\n');
        data->temperatureString = readUntil(fd,'\n');
        data->humidityString = readUntil(fd,'\n');
        data->pressureString = readUntil(fd,'\n');
        data->precipitationString = readUntil(fd,'\n');
    }
    close(fd);
}

void showStationData(StationData* station){
    print(station->dateString);
    print(EOL);
    print(station->hourString);
    print(EOL);
    print(station->temperatureString);
    print(EOL);
    print(station->humidityString);
    print(EOL);
    print(station->pressureString);
    print(EOL);
    print(station->precipitationString);
    print(EOL);
}

void freeDataStation(StationData* data){
    free(data->dateString);
    free(data->hourString);
    free(data->temperatureString);
    free(data->humidityString);
    free(data->pressureString);
    free(data->precipitationString);
}

int sendJackData(StationData* station, int fdSocket){
    //Parse station information to string
    char buffer[100];
    int type;
    buffer[0] = '\0';

    stationToString(station, buffer);

    type = protocolSend(fdSocket, 'D', buffer);
    
    switch (type){
    case 0:
        print("Data sent successfully\n");
        break;
    
    case -1:
        print("Error sending data\n");
        break;

    case -2:
        print("Error reaching Jack\n");
        break; 
    }
    
    return type;
}


//PUBLIC FUNCTIONS
int processConfig(Data* data, const char* file){
    char* aux;
    int fd = -1;

    fd = open(file, O_RDONLY);
    if (fd < 0) {
        print(ERROR_CONFIG);
        return 0;
    }
    else{
        //Fill structure
        data->station = readUntil(fd, '\n');
        data->path = readUntil(fd, '\n');

        aux = readUntil(fd, '\n');
        data->time = atoi(aux);

        data->jack.ip = readUntil(fd, '\n');

        aux = readUntil(fd, '\n');
        data->jack.port = atoi(aux);

        data->wendy.ip = readUntil(fd, '\n');

        aux = readUntil(fd, '\n');
        data->wendy.port = atoi(aux);

        close(fd);
    }
    return 1;
}

void freeConfig(Data* data){
    free(data->station);
    free(data->path);
    free(data->jack.ip);
    free(data->wendy.ip);

    data->station = NULL;
    data->path = NULL;
    data->jack.ip = NULL;
    data->wendy.ip = NULL;
}

int connectToJack(Data* data, Station station){
    int sockfd = -1;

    print(CONNECTING_JACK);

    //Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0){
        print(ERROR_SOCKET);
        return -1;
    }

    struct sockaddr_in s_addr;

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(data->jack.port);
    s_addr.sin_addr.s_addr = inet_addr(data->jack.ip);

    //Connect to socket
    if (connect(sockfd, (void *) &s_addr, sizeof(s_addr)) < 0){
        print(ERROR_CONNECT);
        return -1;
    }

    //Communication protocol
    if (protocolConnection(sockfd, station.name) < 0){
        print(ERROR_CONNECT);
        return -1;
    }
    
    print(CONNECTED_JACK);
    return sockfd;
}

int scanDirectory(Data* data, int fdSocket){
    DIR* d;
    char buffer[32];
    struct dirent* dir = NULL;
    char** files = NULL;
    int num_files = 0;
    StationData station;

    files = (char**) malloc(sizeof(char*));

    //check for new files
    sprintf(buffer, DANNY_PROMPT, data->station);
    print(buffer);
    print(TESTING);

    sprintf(buffer, ".%s", data->path);
    d = opendir(buffer);
    if (d){
        while ((dir = readdir(d)) != NULL){
            files[num_files++] = (char*) malloc((strlen(dir->d_name) + 1) * sizeof(char));
            strcpy(files[num_files - 1], dir->d_name);
            files = realloc(files, (num_files + 1) * sizeof(char*));
        }
        closedir(d);

        //Show directory scan file results
        if (num_files <= 2) print(NO_FILES);
        else {
            //We ignore the . and .. file accesses
            sprintf(buffer, FILES_FOUND, num_files - 2);
            print(buffer);
            for (int i = 2; i < num_files; i++){
                print(files[i]);
                print(EOL);
            }

            //Process files
            for (int i = 2; i < num_files; i++){
                switch (checkExtension(files[i])){
                    case TXT:
                        print(EOL);
                        print(files[i]);
                        print(EOL);

                        sprintf(buffer, ".%s/%s", data->path, files[i]);
                        getStationData(buffer, &station);
                        showStationData(&station);

                        //remove(buffer);

                        //Send to Jack
                        if(sendJackData(&station, fdSocket) == -2) return -1;

                        freeDataStation(&station);
                        break;

                    case JPG:
                        //Send to Wendy

                        break;

                    default:
                        break;
                }
            }
        }

        //Free remaining dynamic memory
        for (int i = 0; i < num_files; i++){
            free(files[i]);
        }
        free(files);
    }

    print(EOL);
    return 0;
}

void disconnectJack(Station* station){
    protocolSend(station->sockfd, 'Q', station->name);
    print(DISCONNECT_JACK);
}
