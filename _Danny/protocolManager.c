/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "protocolManager.h"

//PRIVATE FUNCTIONS
void frameToString(Frame frame, char* final){
    int flag = 0;
    int i;
    bzero(final, 115);

    for (i = 0; i < 14; i++){
        if (frame.source[i] == '\0') flag = 1;
        if (flag) final[i] = '$';
        else final[i] = frame.source[i];
    }
    
    final[14] = frame.type;

    flag = 0;
    for (i = 15; i < 115; i++){
        if (frame.data[i-15] == '\0') flag = 1;
        if (flag) final[i] = '$';
        else final[i] = frame.data[i-15];
    }
}

Frame makeFrame(char type, char* data){
    Frame frame;
    strcpy(frame.source, DANNY);
    frame.type = type;
    strcpy(frame.data, data);

    return frame;
}

int checkFrame(char* frame, char type, char* out){
    Frame newFrame;

    //Get source
    for (int i = 0; i < 14; i++){
        if (frame[i] == '$'){
            newFrame.source[i] = '\0';
            break;
        }
        newFrame.source[i] = frame[i];
    }

    //Get type
    newFrame.type = frame[14];

    //Get data
    for (int i = 15; i < 115; i++){
        if (frame[i] == '$'){
            newFrame.data[i-15] = '\0';
            break;
        }
        else{
            newFrame.data[i-15] = frame[i];
        }
    }

    strcpy(out, newFrame.data);
    if (strcmp(newFrame.source, JACK) != 0) return -1;
    if (newFrame.type != type) return -1;

    return 1;
}


//PUBLIC FUNCTIONS
int protocolConnection(int sockfd, char* name){
    //Connection Request
    Frame frame;
    char buffer[116];
    char aux[116];

    frame = makeFrame('C', name);

    frameToString(frame, buffer);
    buffer[115] = '\0';

    //printf("Sending: -%s- from %d\n", buffer, sockfd);
    write(sockfd, buffer, 115);

    //Connection Reply
    read(sockfd, buffer, 115);

    if (checkFrame(buffer, 'O', aux) > 0) return 0;
    else return -1;
}

void stationToString(StationData* station, char* out){
    strcat(out, station->dateString);
    strcat(out, "#");
    strcat(out, station->hourString);
    strcat(out, "#");
    strcat(out, station->temperatureString);
    strcat(out, "#");
    strcat(out, station->humidityString);
    strcat(out, "#");
    strcat(out, station->pressureString);
    strcat(out, "#");
    strcat(out, station->precipitationString);
    strcat(out, "\0");
}

int protocolSend(int sockfd, char type, char* data){
    Frame frame;
    char buffer[116];
    char aux[116];

    frame = makeFrame(type, data);

    frameToString(frame, buffer);
    buffer[115] = '\0';
    printf("Sending: -%s- from %d\n", buffer, sockfd);
    write(sockfd, buffer, 115);

    //Connection Reply

    if(type == 'D'){
        read(sockfd, buffer, 115);
        if (checkFrame(buffer, 'B', aux) > 0) return 0;
    }

    return -1;
}
