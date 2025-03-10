#include "OBU_LCD.h"
#include "LCD_Driver.h"
#include "icons_included.h"
#define DATA_LENGTH 512


extern bool flag_fee;
extern bool sttGNSS;
extern bool sttSimcom;
extern uint8_t messfmqtt[DATA_LENGTH];
/* OBU_LCD entry function */
/* pvParameters contains TaskHandle_t */
void OBU_LCD_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    char strFee[20];

    /* TODO: add your own code here */
    while (1)
    {
        SEGGER_RTT_printf(0,"thread LCD \n");

        if(flag_fee){
            flag_fee = false;
            sprintf(strFee, "%s", messfmqtt);
            LCD_Draw_String(40, 65, WHITE, BLACK, strFee, 4);
        }

        if (sttGNSS)
        {
            LCD_Draw_String(15, 185, WHITE, BLACK, "status ublox:  OK   ", 2);
            LCD_Draw_Icon(245, 85, 70, 70, icon_GPS_On);
        }
        else
        {
            LCD_Draw_String(15, 185, WHITE, BLACK, "status ublox:  ERROR", 2);
            LCD_Draw_Icon(245, 85, 70, 70, icon_GPS_Off);
        }
        if (!sttSimcom)
        {
            LCD_Draw_String(15, 210, WHITE, BLACK, "status simcom: ERROR", 2);
            LCD_Draw_Icon(245, 160, 70, 70, icon_4G_Off);
        }
        else{
            LCD_Draw_String(15, 210, WHITE, BLACK, "status simcom: OK   ", 2);
            LCD_Draw_Icon(245, 160, 70, 70, icon_4G_On);
        }

        vTaskSuspend(NULL);
        //vTaskDelay(100);
    }
}
