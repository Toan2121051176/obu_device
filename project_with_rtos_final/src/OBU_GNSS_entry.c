#include <OBU_GNSS.h>
#include "RTT/SEGGER_RTT.h"
#include "RTT/SEGGER_RTT_Conf.h"
#include <string.h>
#include "stdio.h"
#include "nmea_parse.h"
#include <stdbool.h>
#include <task.h>





#define DATA_LENGTH 512
#define RESET 0
extern SemaphoreHandle_t g_new_mutex0;
extern char buffer_ss[DATA_LENGTH];
extern TaskHandle_t OBU_LCD;

extern eTaskState eTaskGetState( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

GPS gps_data;
char str_gnss[20] = {"no GPS"};

bool sttGNSS = false;
bool presttGNSS = false;

void uart_ublox_callback(uart_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    (void)p_args;
}

/* thread2 entry function */
/* pvParameters contains TaskHandle_t */
void OBU_GNSS_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    //DMA
//    g_transfer2_cfg.p_info->p_src = (void*)&R_SCI4->RDR;
//    g_transfer2_cfg.p_info->p_dest = &buffer_ss[0];
//    R_DMAC_Open(&g_transfer2_ctrl, &g_transfer2_cfg);
//    R_DMAC_Enable(&g_transfer2_ctrl);

    /* TODO: add your own code here */
    while (1)
    {
        SEGGER_RTT_printf(0,"thread GNSS loop\n");

        presttGNSS = sttGNSS;

        xSemaphoreTake(g_new_mutex0,portMAX_DELAY);
        char *buffer_ss_copy = strdup(buffer_ss);
        xSemaphoreGive(g_new_mutex0);

        nmea_parse(&gps_data, buffer_ss_copy);

        if (gps_data.fix == 1)
        {
            sttGNSS = true;
            xSemaphoreTake(g_new_mutex0,portMAX_DELAY);
            sprintf(str_gnss, "\n%.6f, %.6f\n", gps_data.latitude, gps_data.longitude);
            xSemaphoreGive(g_new_mutex0);
            SEGGER_RTT_printf(0, "\n%s\n",str_gnss);
        }
        else
        {
            sttGNSS = false;
            SEGGER_RTT_printf(0, "\n No fix\n");
        }

        if(presttGNSS != sttGNSS){
            eTaskState statusthread = eTaskGetState(OBU_LCD);
            if(statusthread == eSuspended)
                vTaskResume(OBU_LCD);
        }
        free(buffer_ss_copy);
        vTaskDelay(100);
    }
}
