#include "hal_data.h"
#include <string.h>
#include <stdlib.h>

typedef struct {
    double latitude;
    char latSide;
    double longitude;
    char lonSide;
    float altitude;
    int satelliteCount;
    float hdop;
    int fix;
    char lastMeasure[20];
} GPS;

void nmea_parse(GPS *gps_data, char *buffer);


