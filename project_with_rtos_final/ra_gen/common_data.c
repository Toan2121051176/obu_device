/* generated common source file - do not edit */
#include "common_data.h"
ioport_instance_ctrl_t g_ioport_ctrl;
const ioport_instance_t g_ioport =
{ .p_api = &g_ioport_on_ioport, .p_ctrl = &g_ioport_ctrl, .p_cfg = &g_bsp_pin_cfg, };
SemaphoreHandle_t g_new_mutex0;
#if 0
                StaticSemaphore_t g_new_mutex0_memory;
                #endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
void g_common_init(void)
{
    g_new_mutex0 =
#if 0
                #if 0
                xSemaphoreCreateRecursiveMutexStatic(&g_new_mutex0_memory);
                #else
                xSemaphoreCreateRecursiveMutex();
                #endif
                #else
#if 0
                xSemaphoreCreateMutexStatic(&g_new_mutex0_memory);
                #else
            xSemaphoreCreateMutex ();
#endif
#endif
    if (NULL == g_new_mutex0)
    {
        rtos_startup_err_callback (g_new_mutex0, 0);
    }
}
