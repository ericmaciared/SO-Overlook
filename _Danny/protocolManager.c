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
        if (flag) final[i] = '\0';
        else final[i] = frame.source[i];
    }

    final[14] = frame.type;

    for (i = 15; i < 115; i++){
        final[i] = frame.data[i-15];
    }

    final[115] = '\0';
}

Frame makeFrame(char type, char* data){
    Frame frame;
    bzero(frame.source, 14);
    bzero(frame.data, 100);
    strcpy(frame.source, DANNY);

    frame.type = type;

    for (int i = 0; i < 99; i++){
        frame.data[i] = data[i];
    }

    return frame;
}

int checkFrame(char* frame, char type, char* out){
    Frame newFrame;

    //Get source
    for (int i = 0; i < 14; i++){
        if (frame[i] == '\0' || frame[i] == '$'){
            newFrame.source[i] = '\0';
            break;
        }
        newFrame.source[i] = frame[i];
    }

    //Get type
    newFrame.type = frame[14];

    //Get data
    for (int i = 15; i < 115; i++){
        if (frame[i] == '\0' || frame[i] == '$'){
            newFrame.data[i-15] = '\0';
            break;
        }
        else{
            newFrame.data[i-15] = frame[i];
        }
    }

    strcpy(out, newFrame.data);
    if ((strcmp(newFrame.source, JACK) != 0)
    && (strcmp(newFrame.source, WENDY) != 0)) return -1;
    if (newFrame.type != type) return -1;

    return 1;
}


//PUBLIC FUNCTIONS
int protocolConnection(int sockfd, char* name){
    //Connection Request
    Frame frame;
    char buffer[116];
    char aux[128];

    bzero(buffer, 116);
    bzero(aux, 128);

    frame = makeFrame('C', name);

    frameToString(frame, buffer);
    buffer[115] = '\0';

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
    char buffer[115];
    char aux[128];
    bzero(buffer, 115);
    bzero(aux, 128);

    //Make frame
    frame = makeFrame(type, data);

    //Serialize frame
    frameToString(frame, buffer);
    print(EOL);


    //Send frame
    if(write(sockfd, buffer, 115) != 115) return -1;
    bzero(buffer, 115);

    //Get reply for data transmissions
    if(type == 'D'){
        read(sockfd, buffer, 115);

        print("-");
        for (int i = 0; i < 115; i++) {
            if (buffer[i] == 0)print("!");
            else write(1, &buffer[i], 1);
        }

        print("-\n");

        if (checkFrame(buffer, 'B', aux) > 0) return 0;
    }

    //No reply for image transmissions
    if (type == 'F' || type == 'I') return 0;

    return -1;
}

int protocolReceive(int sockfd){
    char buffer[128];
    char aux[128];
    struct pollfd pfd;

    pfd.fd = sockfd;
    pfd.events = POLLIN | POLLHUP;

    while (1){
        if (poll(&pfd, 1, -1) >= 0){
            if (pfd.revents & POLLIN) {
                read(sockfd, buffer, 115);
                break;
            }
            if (pfd.revents & POLLHUP) break;
        }
    }

    if (pfd.revents & POLLHUP) return -1;
    if (checkFrame(buffer, 'S', aux) > 0) return 0;
    if (checkFrame(buffer, 'R', aux) > 0) return -1;

    return -1;
}
