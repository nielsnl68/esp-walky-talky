#pragma once

#include "I2SAudio.h"
#include <freertos/FreeRTOS.h>
#include <freertos/stream_buffer.h>
#include <esp_now.h>

class EspNowTransport: public I2SAudio {
public:
  EspNowTransport(uint8_t wifi_channel, size_t buffer_size);

  void start() override;
  //void stop() override;
  size_t read(int16_t *samples, size_t count) override;
  size_t write(int16_t *samples, size_t count) override;

  //bool is_empty() override;
  //bool is_full() override;

  friend void receiveCallback(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);

  void on_receive(const esp_now_recv_info_t *recv_info, const uint8_t *data, size_t len);

  bool valid_header(const uint8_t *data, int len);

protected:
  uint8_t wifi_channel_;
  uint8_t *buffer_ = NULL;
  int buffer_size_ = 0;
  int header_size_;

  StreamBufferHandle_t receive_stream_;

};
