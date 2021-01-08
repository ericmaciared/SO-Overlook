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
        if (frame[i] == '\0' || frame[i] == '$'){
            newFrame.data[i-15] = '\0';
            break;
        }
        else{
            newFrame.data[i-15] = frame[i];
        }
    }

    strcpy(out, newFrame.data);
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

void stringToStation(char* string, StationData* station){
    char buffer[16];
    int i = 0;
    int j = 0;

    //Parse Date
    for (j = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; j++){
        buffer[j] = string[i];
        i++;
    }
    i++;
    buffer[j] = '\0';

    station->dateString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->dateString, buffer);

    //Parse Hour
    for (j = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; j++){
        buffer[j] = string[i];
        i++;
    }
    i++;
    buffer[j] = '\0';

    station->hourString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->hourString, buffer);

    //Parse Temperature
    for (j = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; j++){
        buffer[j] = string[i];
        i++;
    }
    i++;
    buffer[j] = '\0';

    station->temperatureString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->temperatureString, buffer);

    //Parse Humidity
    for (j = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; j++){
        buffer[j] = string[i];
        i++;
    }
    i++;
    buffer[j] = '\0';

    station->humidityString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->humidityString, buffer);

    //Parse Pressure
    for (j = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; j++){
        buffer[j] = string[i];
        i++;
    }
    i++;
    buffer[j] = '\0';

    station->pressureString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->pressureString, buffer);

    //Parse Precipitation
    for (j = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; j++){
        buffer[j] = string[i];
        i++;
    }
    buffer[j] = '\0';

    station->precipitationString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->precipitationString, buffer);
}

int checkStation(StationData* data){
    char aux[64];

    //Check Date regex
    strcpy(aux, data->dateString);
    if(strlen(aux)!=10 || aux[2] != '/' ||  aux[5] != '/') return -1;

    //Check Hour regex
    strcpy(aux, data->hourString);
    if(strlen(aux)!=8 || aux[2] != ':' ||  aux[5] != ':') return -1;

    //Check Temperature regex
    strcpy(aux, data->temperatureString);
    if(strlen(aux)>5) return -1;

    //Check Humidity regex
    strcpy(aux, data->humidityString);
    if(strlen(aux)>3) return -1;

    //Check Pressure regex
    strcpy(aux, data->pressureString);
    if(strlen(aux)>6) return -1;

    //Check Precipitation regex
    strcpy(aux, data->precipitationString);
    if(strlen(aux)>4)return -1;


    return 0;
}

Frame makeFrame(char type, char* data){
    Frame frame;
    strcpy(frame.source, JACK);
    frame.type = type;
    strcpy(frame.data, data);

    return frame;
}


//PUBLIC FUNCTIONS
int protocolConnection(int sockfdclient, char* out){
    Frame frame;
    char buffer[116];
    //char aux[128];
    bzero(buffer, 116);

    //Connection Request
    read(sockfdclient, &buffer, 115);

    /*print("Received: ");

    for (int i = 0; i < 115; i++){
        sprintf(aux, "-%c-", buffer[i]);
        print(aux);
    }*/


    if (checkFrame(buffer, 'C', out) > 0){
        bzero(buffer, 115);
        strcpy(frame.source, JACK);
        frame.type = 'O';
        strcpy(frame.data, OK);

        frameToString(frame, buffer);
        write(sockfdclient, buffer, 115);
        return 0;
    }
    else{
        bzero(buffer, 115);
        strcpy(frame.source, JACK);
        frame.type = 'O';
        strcpy(frame.data, ERROR);

        frameToString(frame, buffer);
        write(sockfdclient, buffer, 115);
        //free(buff);
        return -1;
    }
}

char protocolRead(int sockfdclient, StationData* station){
    char buffer[116];
    char aux[116];
    bzero(buffer, 0);
    bzero(aux, 0);

    //Connection Request
    read(sockfdclient, buffer, 115);
    buffer[115] = 0;

    print("Received: ");

    for (int i = 0; i < 115; i++){
        sprintf(aux, "-%c-", buffer[i]);
        print(aux);
    }

    //Check if atmospheric data
    if (checkFrame(buffer, 'D', aux) > 0){
        //Parse string to Station Data
        stringToStation(aux, station);

        //Check station data
        if (checkStation(station) < 0) return 'K';
        return 'D';
    }

    //Check if disconnection frame
    if (checkFrame(buffer, 'Q', aux) > 0) return 'Q';

    return 'Z';
}

void protocolResponse(int sockfdclient, char responseType, char* response){
    Frame frame;
    char buffer[115];

    //Reply with responseType
    strcpy(frame.source, JACK);
    frame.type = responseType;
    strcpy(frame.data, response);

    frameToString(frame, buffer);
    write(sockfdclient, buffer, 115);
}
