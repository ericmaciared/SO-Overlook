/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/

#ifndef _DANNYMANAGER_H_
#define _DANNYMANAGER_H_

//LIBRARIES
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>


//MODULES
#include "functions.h"


//DEFINES
#define EOL "\n"
#define TESTING "Testing...\n"
#define FILES_FOUND "%d files found\n"
#define NO_FILES "No files available\n"
#define ERROR_CONFIG "Error opening configuration file.\n\n"
#define ERROR_ARGS "Error processing arguments.\n\n"

#define STARTING "\nStarting Danny...\n\n"
#define CONNECT_JACK "\nConnecting Jack...\n"
#define DISCONNECT_JACK "\nDisconnecting Jack...\n"
#define DISCONNECT_DANNY "Disconnecting Danny...\n\n"

#define NOFILE 0
#define TXT 1
#define JPG 2


//STRUCTS
typedef struct{
    char* ip;
    int port;
}Config;

typedef struct Data{
  char* station;
  char* path;
  int time;
  Config jack;
  Config wendy;
}Data;

typedef struct StationData{
  char* dateString;
  char* hourString;
  char* temperatureString;
  char* humidityString;
  char* pressureString;
  char* precipitationString;

  int date[8];
  int hour[6];
  float temperature;
  int humidity;
  float pressure;
  float precipitation;
}StationData;

//FUNCTIONS

/*
* Processes the data from a config.txt file and store it
* @param: data points to the Data struct to fill
*         file indicates file address to read from
* @return: 1 if successful 0 if error
*/
int processConfig(Data* data, const char* file);

/*
* Frees all the stored dynamic config data
* @param: data pointer to structure
*/
void freeConfig(Data* data);

/*
* Scans for new files in stored data directory and handles
* sending and processing of information
* @param: data pointer to structure with config information
*/
void scanDirectory(Data* data);


#endif
