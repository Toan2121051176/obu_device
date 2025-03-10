// uart_mqtt_helpers.c
#include "uart_mqtt.h"
#include <stdbool.h>

uint8_t g_temp_buffer_simcom[DATA_LENGTH] = {RESET_VALUE};
uint8_t buffer[DATA_LENGTH] = {RESET_VALUE};

volatile uint8_t g_counter_var_simcom = RESET_VALUE;
volatile uint8_t g_data_received_flag = false;
uint8_t csqfsim[DATA_LENGTH] = {RESET_VALUE};
uint8_t messfmqtt[DATA_LENGTH] = {RESET_VALUE};
uint8_t errorfsim[DATA_LENGTH] = {RESET_VALUE};

extern volatile bool g_uart_event;


/**
 * @brief Stores the received MQTT message into a global buffer.
 */
void message_from_MQTT(int counter) {
    strcpy((char *)messfmqtt, (char *)g_temp_buffer_simcom);
    messfmqtt[counter-2] = '\0';
}
void csq_from_sim(int counter) {
    strcpy((char *)csqfsim, (char *)g_temp_buffer_simcom);
    csqfsim[counter-2] = '\0';
}
void error_from_sim(int counter){
    strcpy((char*)errorfsim,(char *)g_temp_buffer_simcom);
    errorfsim[counter-2]= '\0';
}


/**
 * @brief Delays execution for the specified number of milliseconds.
 *
 * @param[in] time The number of milliseconds to delay.
 */
void delayms(uint32_t time) {
    R_BSP_SoftwareDelay(time, BSP_DELAY_UNITS_MILLISECONDS);
}

/**
 * @brief Sends a UART command and waits for the specified delay.
 *
 * @param[in] command The command string to send.
 * @param[in] delay_ms The delay to wait after sending the command, in milliseconds.
 * @return fsp_err_t Returns FSP_SUCCESS on success, or an error code.
 */
fsp_err_t send_uart_command(const char *command, uint32_t delay_ms) {
    strcpy((char *)buffer, command);

    fsp_err_t status = R_SCI_UART_Write(&g_uart9_simcom_ctrl, buffer, strlen((char *)buffer));

    if (status != FSP_SUCCESS) {
        SEGGER_RTT_printf(0, "Error sending command: %s\n", command);
        return status;
    }

    while(g_uart_event != true);
    g_uart_event = false;
    delayms(delay_ms);
    return FSP_SUCCESS;
}



fsp_err_t setupMQTT(){
    fsp_err_t volatile status;
    send_uart_command("AT+RESET\r\n",8000);

    status = send_uart_command("AT+CMQTTSTART\r\n", 400);
    if(status != FSP_SUCCESS) return status;

    status = send_uart_command("AT+CMQTTACCQ=0,\"chiennv\",0\r\n", 400);
    if(status != FSP_SUCCESS) return status;

    status = send_uart_command("AT+CMQTTCONNECT=0,\"tcp://broker.emqx.io:1883\",60,1\r\n", 2000);
    if(status != FSP_SUCCESS) return status;

    status = send_uart_command("AT+CMQTTSUBTOPIC=0,10,0\r\n", 400);
    if(status != FSP_SUCCESS) return status;

    status = send_uart_command("testelcoml", 400);
    if(status != FSP_SUCCESS) return status;

    status = send_uart_command("AT+CMQTTSUB=0,0\r\n",400);
    if(status != FSP_SUCCESS) return status;

    return FSP_SUCCESS;
}



/**
 * @brief Publishes a message to MQTT.
 *
 * @param[in] message The message to publish.
 * @return fsp_err_t Returns FSP_SUCCESS on success, or an error code.
 */
fsp_err_t pub_mess(const char *message) {

    delayms(100);

    send_uart_command("AT+CMQTTTOPIC=0,9\r\n", 200);
    send_uart_command("testelcol", 200);

    fsp_err_t status;
    char payload_command[22];

    // Calculate the size of the message
    int message_size = strlen(message);

    // Create the payload command with the correct size
    snprintf(payload_command, sizeof(payload_command), "AT+CMQTTPAYLOAD=0,%d\r\n", message_size);

    // Send the payload command
    status = send_uart_command(payload_command, 200);
    if (status != FSP_SUCCESS) {
        return status;
    }

    // Send the actual message
    status = send_uart_command(message, 200);
    if (status != FSP_SUCCESS) {
        return status;
    }

    // Send the publish command
    status = send_uart_command("AT+CMQTTPUB=0,1,60,0\r\n", 200);
    if (status != FSP_SUCCESS) {
        return status;
    }

    return FSP_SUCCESS;
}
