/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "jackManager.h"

//PRIVATE FUNCTIONS
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

void freeStationData(StationData* data){
    free(data->dateString);
    free(data->hourString);
    free(data->temperatureString);
    free(data->humidityString);
    free(data->pressureString);
    free(data->precipitationString);
}

//PUBLIC FUNCTIONS
int processConfig(Config* config, const char* file){
    int fd = -1;
    char* buffer;

    fd = open(file, O_RDONLY);

    if (fd <= 0) {
        perror(ERROR_CONFIG);
        return 0;
    }
    else {
        config->ip = readUntil(fd, '\n');
        buffer = readUntil(fd, '\n');
        config->port = atoi(buffer);
    }
    return 1;
}

int initServer(Config* config){
    int sockfd;
    struct sockaddr_in s_addr;

    //Create and check socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0){
        print(ERROR_SOCKET);
        return -1;
    }

    //Bind and check socket
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(config->port);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (void *) &s_addr, sizeof(s_addr)) != 0){
        print(ERROR_BIND);
        return -1;
    }

    //Listen
    listen(sockfd, 16);

    return sockfd;
}

int acceptConnection(int sockfdServer, Station* client){
    struct sockaddr_in s_addr;
    socklen_t len = sizeof(s_addr);
    char buff[128];

    print(JACK_PROMPT);
    print(CONNECTION_WAITING);

    //Waits for connection from client
    client->sockfd = accept(sockfdServer, (void *) &s_addr, &len);
    if (client->sockfd < 0){
        print(ERROR_ACCEPT);
        return -1;
    }

    //Communication protocol established
    if(protocolConnection(client->sockfd, buff) < 0){
        print(ERROR_ACCEPT);
        return -1;
    }

    client->name = (char*) malloc(strlen(buff) * sizeof(char));
    strcpy(client->name, buff);

    sprintf(buff, NEW_CONNECTION, client->name);
    print(buff);
    return 0;
}

char readFromDanny(Station* client){
    StationData sd;
    char type = protocolRead(client->sockfd, &sd);

    //Read frame
    print(JACK_PROMPT);
    print(RECEIVING_DATA);
    switch(type){
        case 'D':
            //Send data Lloyd
            //Print to screen
            showStationData(&sd);

            //Frees of dynamic memory
            freeStationData(&sd);
            break;

        case 'Q':
            break;

        default:
            //Erroneous frame

            return 'K';
            break;
    }
    return type;
}

int replyToDanny(Station* client, char type){
    char buffer[64];
    switch(type){
        case 'D':
            protocolResponse(client->sockfd, 'B', DATAOK);

            return 0;

        case 'K':
            protocolResponse(client->sockfd, 'K', DATAKO);

            return 0;

        case 'Q':
            //Disconnect Server
            sprintf(buffer, BYEDANNY, client->name);
            print(buffer);
            close(client->sockfd);
            free(client->name);
            free(client);

            return 1;

        default:
            //Erroneous processing
            //Reply with X (Custom control variable)
            protocolResponse(client->sockfd, 'X', DATADEAD);
            print(DATADEAD);
            print(EOL);

            //Disconnect Server
            close(client->sockfd);
            free(client->name);
            free(client);

            return 1;
    }
    return -1;
}
