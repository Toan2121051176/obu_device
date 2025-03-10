#include "OBU_INIT.h"
#include "stdio.h"
#include "LCD_Driver.h"
#include "icons_included.h"

#define DATA_LENGTH 512
#define RESET 0


extern char buffer_ss[DATA_LENGTH];
extern TaskHandle_t OBU_GNSS;
extern TaskHandle_t OBU_MQTT;
extern TaskHandle_t OBU_RECONNECT;
extern SemaphoreHandle_t g_new_mutex0;


/* Callback function */
void spi_lcd_callback(spi_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    (void)p_args;
}



/* OBU_INIT entry function */
/* pvParameters contains TaskHandle_t */
void OBU_INIT_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);




    /* TODO: add your own code here */
    while (1)
    {



        xSemaphoreTake(g_new_mutex0,portMAX_DELAY);
        LCD_Init();
        SEGGER_RTT_printf(0,"LCD done!");
        LCD_Set_Rotation(3);
        LCD_Fill_Screen(BLACK);
        LCD_Draw_Filled_Rectangle(WHITE, 0, 115, 239, 119);
        LCD_Draw_Filled_Rectangle(WHITE, 240, 0, 244, 239);
        LCD_Draw_String(20, 20, WHITE, BLACK, "Fee:", 4);
//        LCD_Draw_String(15, 135, WHITE, BLACK, "latitude:", 2);
//        LCD_Draw_String(15, 160, WHITE, BLACK, "longitude:", 2);
        LCD_Draw_String(15, 185, WHITE, BLACK, "status ublox:", 2);
        LCD_Draw_String(15, 210, WHITE, BLACK, "status simcom:", 2);

        LCD_Draw_Icon(245, 10, 70, 70, icon_Power_On);
        LCD_Draw_Icon(245, 160, 70, 70, icon_4G_On);
        LCD_Draw_Icon(245, 85, 70, 70, icon_GPS_On);


        //Open uart
        fsp_err_t status = R_SCI_UART_Open(&g_uart9_simcom_ctrl, &g_uart9_simcom_cfg);
        if(status == FSP_SUCCESS){
            SEGGER_RTT_printf(0, "uart simcom ok\n");
        }
        else{
            SEGGER_RTT_printf(0, "ko khoi tao dc uart simcom\n");
        }
        R_SCI_UART_Open(&g_uart4_ublox_ctrl, &g_uart4_ublox_cfg);

        // DMA
        g_transfer2_cfg.p_info->p_src = (void*)&R_SCI4->RDR;
        g_transfer2_cfg.p_info->p_dest = &buffer_ss[0];

        R_DMAC_Open(&g_transfer2_ctrl, &g_transfer2_cfg);
        R_DMAC_Enable(&g_transfer2_ctrl);

        //Open lcd
        xSemaphoreGive(g_new_mutex0);

        vTaskDelete(NULL);
    }
}
