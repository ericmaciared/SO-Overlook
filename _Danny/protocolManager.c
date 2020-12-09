/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "protocolManager.h"

//PRIVATE FUNCTIONS
char* frameToString(Frame frame){
    char* final = NULL;
    final = (char*) malloc(115 * sizeof(char));

    for (int i = 0; i < 115; i++){
        final[i] = '\0';
    }

    strcat(final, frame.source);
    memset(&final[strlen(final)], '$', 14 - strlen(frame.source));
    final[14] = frame.type;


    strcat(final, frame.data);
    memset(&final[strlen(final)], '$', 100 - strlen(frame.data));

    return final;
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
    strcpy(frame.source, DANNY);
    frame.type = 'C';
    strcpy(frame.data, name);

    char* aux = frameToString(frame);
    printf("Sending: -%s- from %d\n", aux, sockfd);
    write(sockfd, aux, strlen(aux));
    free(aux);

    //Connection Reply
    char buffer[115];
    char buffer2[115];
    read(sockfd, buffer, 115);

    if (checkFrame(buffer, 'O', buffer2) > 0) return 0;
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

    frame = makeFrame(type, data);

    char* aux = frameToString(frame);
    printf("Sending: -%s- from %d\n", aux, sockfd);
    write(sockfd, aux, strlen(aux));
    free(aux);

    //Connection Reply
    char buffer[116];
    char buffer2[116];

    if(type == 'D'){
        read(sockfd, buffer, 115);
        if (checkFrame(buffer, 'B', buffer2) > 0) return 0;
    }
    return -1;
}
