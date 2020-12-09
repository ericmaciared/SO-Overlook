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
    if (strcmp(newFrame.source, DANNY) != 0) return -1;
    if (newFrame.type != type) return -1;
    
    return 1;
}

char* frameToString(Frame frame){
    char* final;
    final = (char*) malloc(115 * sizeof(char));
    
    strcat(final, frame.source);
    memset(&final[strlen(final)], '$', 14 - strlen(frame.source));
    final[14] = frame.type;
    strcat(final, frame.data);
    memset(&final[strlen(final)], '$', 100 - strlen(frame.data));

    return final;
}

void stringToStation(char* string, StationData* station){
    char buffer[16];
    int i = 0;
    int j = 0;

    printf("String: -%s-\n", string);

    for (j = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; j++){
        buffer[j] = string[i];
        i++;
    }
    i++;
    j++;
    buffer[j] = '\0';
    j = 0;

    station->dateString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->dateString, buffer);

    for (j = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; j++){
        buffer[j] = string[i];
        i++;
    }
    i++;
    j++;
    buffer[j] = '\0';
    j = 0;

    station->hourString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->hourString, buffer);

    for (j = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; j++){
        buffer[j] = string[i];
        i++;
    }
    i++;
    j++;
    buffer[j] = '\0';
    j = 0;

    printf("Hour: -%s-\n", station->hourString);

    station->temperatureString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->temperatureString, buffer);

    for (i = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; i++){
        buffer[j] = string[i];
        j++;
    }
    i++;
    j++;
    buffer[j] = '\0';
    j = 0;

    station->humidityString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->humidityString, buffer);

    for (j = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; j++){
        buffer[j] = string[i];
        i++;
    }
    i++;
    j++;
    buffer[j] = '\0';
    j = 0;

    station->pressureString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->pressureString, buffer);

    for (j = 0; string[i] != '#' && string[i] != '$' && string[i] != '\0'; j++){
        buffer[j] = string[i];
        i++;
    }
    j++;
    buffer[j] = '\0';
    j = 0;

    station->precipitationString = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(station->precipitationString, buffer);
}

int checkStation(StationData* data){
    char aux[64];

    //Check Date regex
    strcpy(aux, data->dateString);
    if(strlen(aux)!=10 || aux[2] != '/' ||  aux[5] != '/') return -1;

    printf("Flag1\n");

    //Check Hour regex
    strcpy(aux, data->hourString);
        printf("-%s-\n", aux);

    if(strlen(aux)!=8 || aux[2] != ':' ||  aux[5] != ':') return -1;

    printf("Flag2\n");

    //Check Temperature regex
    strcpy(aux, data->temperatureString);
    if(strlen(aux)>5) return -1;

    printf("Flag3\n");

    //Check Humidity regex
    strcpy(aux, data->humidityString);
    if(strlen(aux)>3) return -1;

    printf("Flag4\n");

    //Check Pressure regex
    strcpy(aux, data->pressureString);
    if(strlen(aux)>6) return -1;

    printf("Flag5\n");

    //Check Precipitation regex
    strcpy(aux, data->precipitationString);
    if(strlen(aux)>4)return -1;

    printf("Flag6\n");


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
    char* buff;

    //Connection Request
    read(sockfdclient, buffer, 115);
    buffer[115] = '\0';

    if (checkFrame(buffer, 'C', out) > 0){
        strcpy(frame.source, JACK);
        frame.type = 'O';
        strcpy(frame.data, OK);

        buff = frameToString(frame);
        write(sockfdclient, buff, strlen(buff));
        free(buff);
        return 0;  
    }
    else{
        strcpy(frame.source, JACK);
        frame.type = 'O';
        strcpy(frame.data, ERROR);

        buff = frameToString(frame);
        write(sockfdclient, buff, strlen(buff));
        free(buff);
        return -1;
    }
}

char protocolRead(int sockfdclient, StationData* station){
    char buffer[116];

    //Connection Request   
    read(sockfdclient, buffer, 116);
    buffer[115] = 0;

    //Check if atmospheric data
    char aux[116];
    if (checkFrame(buffer, 'D', aux) > 0){
        //Parse string to Station Data

        stringToStation(aux, station);

        //Check station data
        if (checkStation(station) < 0) return 'K';
        
        printf("FlagKO\n");
        return 'D';
    }

    //Check if disconnection frame
    if (checkFrame(buffer, 'Q', aux) > 0){
        /* code */
    }

    return 'Z';
}