#include "OBU_MQTT.h"
#include <stdbool.h>
#include "nmea_parse.h"

#define DATA_LENGTH 512
#define RESET 0

bool sttSimcom = false;
bool flag_fee = false;


extern TaskHandle_t OBU_RECONNECT;
extern TaskHandle_t OBU_LCD;

extern char str_gnss[20];
extern GPS gps_data;
GPS gps_fallbackdata;
extern char g_temp_buffer_simcom[DATA_LENGTH];
extern uint16_t g_counter_var_simcom ;
extern SemaphoreHandle_t g_new_mutex0;


/* OBU_MQTT entry function */

volatile bool g_uart_event = false;

/* Callback function */
void uart_simcom_callback(uart_callback_args_t *p_args)
{

    /* TODO: add your own code here */
    switch (p_args->event)
        {
        case UART_EVENT_RX_CHAR:
            //sttSimcom = true;
            uint8_t c = (uint8_t)p_args->data;
            g_temp_buffer_simcom[g_counter_var_simcom++] = c;

            if (c == '\n')
            {
                if (strstr(g_temp_buffer_simcom, "cost"))
                {
                    flag_fee = true;
                    message_from_MQTT(g_counter_var_simcom);
                }
                if (strstr(g_temp_buffer_simcom, "CSQ"))
                {
                    csq_from_sim(g_counter_var_simcom);
                }
                if (strstr(g_temp_buffer_simcom, "ERROR"))
                {
                    error_from_sim(g_counter_var_simcom);
                    gps_fallbackdata = gps_data;
                    sttSimcom = 0;
                    //vTaskResume(OBU_RECONNECT);

                }
                SEGGER_RTT_printf(0, "%s", g_temp_buffer_simcom);
                memset(g_temp_buffer_simcom, 0, sizeof(g_temp_buffer_simcom));
                g_counter_var_simcom = RESET;
            }
            break;
        case UART_EVENT_TX_COMPLETE:
            g_uart_event = true;
            break;
        default:
            break;
        }

}


/* pvParameters contains TaskHandle_t */
void OBU_MQTT_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    vTaskSuspend(OBU_RECONNECT);
    SEGGER_RTT_printf(0,"thread MQTT\n");

    xSemaphoreTake(g_new_mutex0,portMAX_DELAY);
    setupMQTT();
    xSemaphoreGive(g_new_mutex0);
    /* TODO: add your own code here */
    while (1)
    {
        SEGGER_RTT_printf(0,"thread MQTT loop\n");

        if(sttSimcom){
            if(flag_fee)
                vTaskResume(OBU_LCD);
            xSemaphoreTake(g_new_mutex0,portMAX_DELAY);
            pub_mess(str_gnss);
            xSemaphoreGive(g_new_mutex0);
        }
        else{
            vTaskResume(OBU_LCD);
            vTaskResume(OBU_RECONNECT);
        }
        vTaskDelay (100);
    }
}
