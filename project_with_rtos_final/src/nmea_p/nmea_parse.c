#include "nmea_parse.h"


char *data[6];

int gps_checksum(char *nmea_data) {
    // If the input string is too short, return invalid
    if (strlen(nmea_data) < 3) return 0;

    // Locate the position of the '*' character
    char *asterisk = strrchr(nmea_data, '*');
    if (asterisk == NULL || asterisk - nmea_data < 1) return 0;

    // Extract the checksum from the NMEA sentence
    char recv_crc[3];
    recv_crc[0] = asterisk[1];
    recv_crc[1] = asterisk[2];
    recv_crc[2] = '\0';  // Null terminator

    // Convert received checksum to integer
    int receivedHash = strtol(recv_crc, NULL, 16);

    // Compute checksum for characters between '$' and '*'
    int crc = 0;
    for (char *p = nmea_data + 1; *p != '*' && *p != '\0'; p++) {
        crc ^= *p;
    }
    // XOR the calculated CRC with 0x47
    crc ^= 0x47;

    return crc == receivedHash;
}

// Parsing function implementation
int nmea_GNGGA(GPS *gps_data, char *inputString) {
    char *values[25];
    int counter = 0;
    memset(values, 0, sizeof(values));
    char *marker = strtok(inputString, ",");
    while (marker != NULL) {
        values[counter++] = strdup(marker); // Duplicate marker
        marker = strtok(NULL, ",");
    }

    char lonSide = values[5][0];
    char latSide = values[3][0];
    strncpy(gps_data->lastMeasure, values[1], sizeof(gps_data->lastMeasure)-1);
    gps_data->lastMeasure[sizeof(gps_data->lastMeasure)-1] = '\0'; // Ensure null termination

    if (latSide == 'S' || latSide == 'N') {
        char lat_d[3] = {0}; // Increase size to avoid overflow
        char lat_m[8] = {0}; // Include space for null terminator
        strncpy(lat_d, values[2], 2);
        strncpy(lat_m, values[2] + 2, 7);

        int lat_deg_strtol = strtol(lat_d, NULL, 10);
        float lat_min_strtof = atof(lat_m);
        //float lat_min_strtof = strtof(lat_m, NULL);
        double lat_deg = lat_deg_strtol + lat_min_strtof / 60;

        char lon_d[4] = {0}; // Increase size to handle 3 digits
        char lon_m[8] = {0};
        strncpy(lon_d, values[4], 3);
        strncpy(lon_m, values[4] + 3, 7);

        int lon_deg_strtol = strtol(lon_d, NULL, 10);
        float lon_min_strtof = atof(lon_m);
        //float lon_min_strtof = strtof(lon_m, NULL);
        double lon_deg = lon_deg_strtol + lon_min_strtof / 60;

        if (lat_deg != 0 && lon_deg != 0 && lat_deg < 90 && lon_deg < 180) {
            gps_data->latitude = lat_deg;
            gps_data->latSide = latSide;
            gps_data->longitude = lon_deg;
            gps_data->lonSide = lonSide;

            float altitude = atof(values[9]);
            //float altitude = strtof(values[9], NULL);
            gps_data->altitude = altitude != 0 ? altitude : gps_data->altitude;
            gps_data->satelliteCount = strtol(values[7], NULL, 10);

            int fixQuality = strtol(values[6], NULL, 10);
            gps_data->fix = fixQuality > 0 ? 1 : 0;

            float hdop = atof(values[8]);
            //float hdop = strtof(values[8], NULL);
            gps_data->hdop = hdop != 0 ? hdop : gps_data->hdop;
        } else {
            for (int i = 0; i < counter; i++) free(values[i]);
            return 0;
        }
    }

    for (int i = 0; i < counter; i++) free(values[i]);
    return 1;
}

int nmea_GNGSA(GPS *gps_data, char*inputString){
    char *values[25];
    int counter = 0;
    memset(values, 0, sizeof(values));
    char *marker = strtok(inputString, ",");
    while (marker != NULL) {
        values[counter++] = malloc(strlen(marker) + 1); //free later!!!!!!
        strcpy(values[counter - 1], marker);
        marker = strtok(NULL, ",");
    }
    int fix = strtol(values[2], NULL, 10);
    gps_data->fix = fix > 1 ? 1 : 0;
    int satelliteCount = 0;
    for(int i=3; i<15; i++){
        if(values[i][0] != '\0'){
            satelliteCount++;
        }
    }
    gps_data->satelliteCount = satelliteCount;
    for(int i=0; i<counter; i++) free(values[i]);
    return 1;
}



int nmea_GNGLL(GPS *gps_data, char*inputString) {

    char *values[25];
    int counter = 0;
    memset(values, 0, sizeof(values));
    char *marker = strtok(inputString, ",");
    while (marker != NULL) {
        values[counter++] = malloc(strlen(marker) + 1); //free later!!!!!!
        strcpy(values[counter - 1], marker);
        marker = strtok(NULL, ",");
    }
    char latSide = values[2][0];
    if (latSide == 'S' || latSide == 'N') { //check if data is sorta intact
        char lat_d[2];
        char lat_m[7];
        for (int z = 0; z < 2; z++) lat_d[z] = values[1][z];
        for (int z = 0; z < 6; z++) lat_m[z] = values[1][z + 2];

        int lat_deg_strtol = strtol(lat_d, NULL, 10);
        float lat_min_strtof = strtof(lat_m, NULL);
        double lat_deg = lat_deg_strtol + lat_min_strtof / 60;

        char lon_d[3];
        char lon_m[7];
        char lonSide = values[4][0];
        for (int z = 0; z < 3; z++) lon_d[z] = values[3][z];
        for (int z = 0; z < 6; z++) lon_m[z] = values[3][z + 3];

        int lon_deg_strtol = strtol(lon_d, NULL, 10);
        float lon_min_strtof = strtof(lon_m, NULL);
        double lon_deg = lon_deg_strtol + lon_min_strtof / 60;
        //confirm that we aren't on null island
        if(lon_deg_strtol == 0 || lon_min_strtof == 0 || lat_deg_strtol == 0 || lat_min_strtof == 0) {
            for(int i = 0; i<counter; i++) free(values[i]);
            return 0;
        }
        else{
            gps_data->latitude = lat_deg;
            gps_data->longitude = lon_deg;
            gps_data->latSide = latSide;
            gps_data->lonSide = lonSide;
            for(int i = 0; i<counter; i++) free(values[i]);
            return 1;
        }
    }
    else return 0;
}


void nmea_parse(GPS *gps_data, char *buffer) {
    SEGGER_RTT_printf(0, "\nBuffer: \n%s\n", buffer);

    char *data[6] = {0};  // Ensure the array is cleared
    char *token = strtok((char *)buffer, "$");
    int cnt = 0;

    while (token != NULL && cnt < 6) {
        data[cnt] = strdup(token);
        if (data[cnt] == NULL) {
//            SEGGER_RTT_printf(0, "Memory allocation failed for data[%d]\n", cnt);
//            for (int i = 0; i < cnt; i++) free(data[i]);
//            return;
        }
        SEGGER_RTT_printf(0, "Token[%d]: %s\n", cnt, data[cnt]);
        cnt++;
        token = strtok(NULL, "$");
    }

    for (int i = 0; i < cnt; i++) {
        if (strstr(data[i], "\r\n") != NULL && gps_checksum(data[i])) {
            SEGGER_RTT_printf(0, "Valid checksum for data[%d]: %s\n", i, data[i]);
            if (strstr(data[i], "GNGGA") != NULL) {
                if (nmea_GNGGA(gps_data, data[i])) {
                    SEGGER_RTT_printf(0, "Parsed GNGGA successfully\n");
                } else {
                    SEGGER_RTT_printf(0, "Failed to parse GNGGA\n");
                }
            } else if (strstr(data[i], "GNGSA") != NULL) {
                if (nmea_GNGSA(gps_data, data[i])) {
                    SEGGER_RTT_printf(0, "Parsed GNGSA successfully\n");
                } else {
                    SEGGER_RTT_printf(0, "Failed to parse GNGSA\n");
                }
            } else if (strstr(data[i], "GNGLL") != NULL) {
                if (nmea_GNGLL(gps_data, data[i])) {
                    SEGGER_RTT_printf(0, "Parsed GNGLL successfully\n");
                } else {
                    SEGGER_RTT_printf(0, "Failed to parse GNGLL\n");
                }
            }
        } else {
            SEGGER_RTT_printf(0, "Invalid checksum for data[%d]: %s\n", i, data[i]);
        }
        free(data[i]);  // Free allocated memory
    }
}





