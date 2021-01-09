/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "wendyManager.h"

//PRIVATE FUNCTIONS

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
        free(buffer);
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

    print(CONNECTION_WAITING);

    //Waits for connection from client
    client->sockfd = accept(sockfdServer, (void *) &s_addr, &len);

    if (client->sockfd < 0 || errno == EINTR){
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

char readFromDanny(Station* client, char* out){
    char buffer[128];
    char type = protocolRead(client->sockfd, buffer);

    //Read frame
    switch(type){
        //New image
        case 'I':
            for (size_t i = 0; i < 99; i++){
                out[i] = buffer[i];
            }
            sprintf(buffer, RECEIVING_DATA, client->name);
            print(buffer);
            break;

        //Data from image
        case 'F':
            for (int i = 0; i < 99; i++){
                out[i] = buffer[i];
            }
            break;

        //Disconnection
        case 'Q':
            break;

        //Wrong frame
        default:
            replyToDanny(client, 'Z');
            return 'Z';
    }

    return type;
}

int replyToDanny(Station* client, char type){
    switch(type){
        case 'S':
            protocolResponse(client->sockfd, 'S', IMGOK);
            break;

        case 'R':
            protocolResponse(client->sockfd, 'R', IMGKO);
            break;

        case 'Z':
            protocolResponse(client->sockfd, 'Z', ERROR_TRAMA);
            break;
    }
    return 0;
}

int dataToImageData(char* in, char* imageName, char* md5sum){
    int i = 0;
    int j = 0;
    char aux[32];

    while (in[i] != '#'){
        imageName[j++] = in[i++];

    }
    imageName[j] = '\0';
    i++; 
    j=0;


    while (in[i] != '#'){
        aux[j++] = in[i++];
    }
    aux[j] = '\0';
    i++; 
    j=0;
    
    while (in[i] != '\0'){
        md5sum[j++] = in[i++];
    }
    md5sum[j] = '\0';
    i++; 
    j=0;

    return atoi(aux);
}

int checkMD5SUM(char* origin, char* current){
    pid_t pid;
    int link[2];
    char* md5sum;

    //Get MD5SUM from file
    if (pipe(link) < 0) return -1;
    if ((pid = fork()) < 0) return -1;

    if (pid == 0){
        char *args[] = {
            "md5sum",
            current,
            NULL
        };
        dup2(link[1], 1);
        execvp(args[0], args);
    }
    else{
        md5sum = readUntil(link[0], ' ');
        wait(NULL);
    }

    close(link[0]);
    close(link[1]);
    
    return strcmp(origin, md5sum);
}
