/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "jackManager.h"

//PRIVATE FUNCTIONS
void readFromClient(int fd, StationData* data){
    print("READING\n");
    char* aux;
    aux = readUntil(fd, '\0');
    print("$");
    print(aux);
    print(EOL);
    free(aux);
    data->dateString = readUntil(fd,'\0');
    print(data->dateString);
    print(EOL);
    data->hourString = readUntil(fd,'\0');
    print(data->hourString);
    print(EOL);
    data->temperatureString = readUntil(fd,'\0');
    print(data->temperatureString);
    print(EOL);
    data->humidityString = readUntil(fd,'\0');
    print(data->humidityString);
    print(EOL);
    data->pressureString = readUntil(fd,'\0');
    print(data->pressureString);
    print(EOL);
    data->precipitationString = readUntil(fd,'\0');
    print(data->precipitationString);
    print(EOL);
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

int acceptConnection(int sockfdServer){
    struct sockaddr_in s_addr;
    socklen_t len = sizeof(s_addr);

    print(CONNECTION_WAITING);
    
    //Waits for connection from client
    int sockfdClient = accept(sockfdServer, (void *) &s_addr, &len);
    if (sockfdClient < 0){
        print(ERROR_ACCEPT);
        return -1;
    }

    //Communication protocol established
    if(protocolConnection(sockfdServer, sockfdClient) < 0){
        print(ERROR_ACCEPT);
        return -1;
    }

    printf("New connection from  %s:%hu\n", inet_ntoa(s_addr.sin_addr),
        ntohs(s_addr.sin_port));

    return sockfdClient;
}
