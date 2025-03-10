// uart_mqtt_.h
#ifndef UART_MQTT_H
#define UART_MQTT_H

#include "hal_data.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define DATA_LENGTH (200u)
#define RESET_VALUE 0

extern uint8_t g_temp_buffer_simcom[DATA_LENGTH];
extern uint8_t buffer[DATA_LENGTH];
extern uint8_t messfmqtt[DATA_LENGTH];
extern volatile uint8_t g_counter_var_simcom;
extern volatile uint8_t g_data_received_flag;
extern uint8_t csqfsim[DATA_LENGTH];
extern uint8_t errorfsim[DATA_LENGTH];
// Function declarations
void message_from_MQTT(int counter);
void csq_from_sim(int counter);
void error_from_sim(int counter);
void delayms(uint32_t time);

fsp_err_t setupMQTT();

fsp_err_t send_uart_command(const char *command, uint32_t delay_ms);
fsp_err_t pub_mess(const char *message);

#endif // UART_MQTT_H
