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
#include <conio.h>


//MODULES
#include "functions.h"


//DEFINES
#define EOL "\n"
#define TESTING "Testing...\n"
#define FILES_FOUND "%d files found\n"
#define NO_FILES "No files available\n"
#define NOFILE 0
#define TXT 1
#define JPG 2


//STRUCTS
typedef struct Data{
  char* station;
  char* path;
  int time;
  char* jackIP;
  char* jackPort;
  char* wendyIP;
  char* wendyPort;
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

//OBJECTIVE: process the data from a config.txt file and store it
void processConfig(Data* data, const char* file);

//OBJECTIVE: free all the stored config data
void freeConfig(Data* data);

//OBJECTIVE: scan local data directory for new files
void scanDirectory(Data* data);


#endif