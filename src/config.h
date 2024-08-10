#pragma once

#include <driver/gpio.h>


#ifdef __cplusplus
extern "C" {
#endif

// sample rate for the system
#define SAMPLE_RATE 16000
#define BUFF_SIZE   2048

// I2S Microphone Settings
// Which channel is the I2S microphone on? I2S_CHANNEL_FMT_ONLY_LEFT or I2S_CHANNEL_FMT_ONLY_RIGHT
// Generally they will default to LEFT - but you may need to attach the L/R pin to GND
// #define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT

#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_19
#define I2S_MIC_SERIAL_DATA GPIO_NUM_23

// speaker settings
#define I2S_SPEAKER_SERIAL_CLOCK GPIO_NUM_19
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_33
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_22
// Shutdown line if you have this wired up or -1 if you don't
#define I2S_SPEAKER_SD_PIN -1

// transmit button
#define GPIO_TRANSMIT_BUTTON GPIO_NUM_39
#define GPIO_BLINK_LED GPIO_NUM_27


#define ESP_NOW_WIFI_CHANNEL 1

// In case all transport packets need a header (to avoid interference with other applications or walkie talkie sets),
// specify TRANSPORT_HEADER_SIZE (the length in bytes of the header) in the next line, and define the transport header in config.cpp
#define TRANSPORT_HEADER_SIZE 3

static const uint8_t transport_header[TRANSPORT_HEADER_SIZE] = {0xC1, 0x99, 0x83};

#ifdef __cplusplus
}
#endif
