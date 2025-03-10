#include "OBU_RECONNECT.h"
#include "nmea_parse.h"
/* OBU_RECONNECT entry function */

extern GPS gps_fallbackdata;
extern bool sttSimcom;
extern TaskHandle_t OBU_RECONNECT;
extern TaskHandle_t OBU_LCD;
extern SemaphoreHandle_t g_new_mutex0;

/* pvParameters contains TaskHandle_t */
void OBU_RECONNECT_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    char str1[25];
    /* TODO: add your own code here */
    while (1)
    {

        SEGGER_RTT_printf(0,"thread try to reconnect\n");
        xSemaphoreTake(g_new_mutex0,portMAX_DELAY);
        setupMQTT();
        sprintf(str1, "GPS_e4_at:% .6f, % .6f\n", gps_fallbackdata.latitude, gps_fallbackdata.longitude);
        pub_mess(str1);
        xSemaphoreGive(g_new_mutex0);
        sttSimcom = true;
        vTaskResume(OBU_LCD);
        vTaskSuspend(OBU_RECONNECT);
        vTaskDelay (100);
    }
}
