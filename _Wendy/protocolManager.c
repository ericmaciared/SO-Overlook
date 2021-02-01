/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "protocolManager.h"

//PRIVATE FUNCTIONS
//Connection frame checking
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
        newFrame.data[i-15] = frame[i];
    }

    for (int i = 0; i < 99; i++){
        out[i] = newFrame.data[i];
    }

    if (strcmp(newFrame.source, DANNY) != 0) return -1;
    if (newFrame.type != type) return -1;

    return 1;
}

void frameToString(Frame frame, char* final){
    int flag = 0;
    bzero(final, 115);

    for (int i = 0; i < 14; i++){
        if (frame.source[i] == 0) flag = 1;
        if (flag) final[i] = '\0';
        else final[i] = frame.source[i];
    }

    final[14] = frame.type;

    flag = 0;
    for (int i = 15; i < 115; i++){
        if (frame.data[i-15] == 0) flag = 1;
        if (flag) final[i] = '\0';
        else final[i] = frame.data[i-15];
    }
}

Frame makeFrame(char type, char* data){
    Frame frame;
    strcpy(frame.source, WENDY);
    frame.type = type;
    for (int i = 0; i < 99; i++){
        frame.data[i] = data[i];
    }

    return frame;
}


//PUBLIC FUNCTIONS
int protocolConnection(int sockfdclient, char* out){
    Frame frame;
    char buffer[116];
    bzero(buffer, 116);

    //Connection Request
    read(sockfdclient, &buffer, 115);
    
    //Connection Response
    if (checkFrame(buffer, 'C', out) > 0){
        bzero(buffer, 115);
        strcpy(frame.source, WENDY);
        frame.type = 'O';
        strcpy(frame.data, OK);

        frameToString(frame, buffer);
        write(sockfdclient, buffer, 115);
        return 0;
    }
    else{
        bzero(buffer, 115);
        strcpy(frame.source, WENDY);
        frame.type = 'O';
        strcpy(frame.data, ERROR);

        frameToString(frame, buffer);
        write(sockfdclient, buffer, 115);
        return -1;
    }
}

char protocolRead(int sockfdclient, char* out){
    char buffer[116];
    char aux[116];
    bzero(buffer, 0);
    bzero(aux, 0);

    //Connection Request
    read(sockfdclient, buffer, 115);
    buffer[115] = 0;

    //Check if new image
    if (checkFrame(buffer, 'I', aux) > 0){
        strcpy(out, aux);
        return 'I';
    }

    //Check if image data
    if (checkFrame(buffer, 'F', aux) > 0){
        for (int i = 0; i < 99; i++){
            out[i] = aux[i];
        }
        return 'F';
    }
    
    //Check if disconnection frame
    if (checkFrame(buffer, 'Q', aux) > 0) return 'Q';

    return 'Z';
}

void protocolResponse(int sockfdclient, char responseType, char* response){
    Frame frame;
    char buffer[115];
    bzero(buffer, 0);

    //Reply with responseType
    strcpy(frame.source, WENDY);
    frame.type = responseType;
    strcpy(frame.data, response);

    frameToString(frame, buffer);
    write(sockfdclient, buffer, 115);
}
