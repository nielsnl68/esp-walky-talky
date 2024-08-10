
#include "EspNowTransport.h"
#include <cinttypes>
#include <cstdio>
#include <algorithm>
#include <stdio.h>
#include <string.h>

#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/stream_buffer.h>
#include <esp_log.h>
#include "config.h"

static const char *const TAG = "ComBadge";

const int MAX_ESP_NOW_PACKET_SIZE = 250;
const uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static EspNowTransport *instance = NULL;


EspNowTransport::EspNowTransport(uint8_t wifi_channel, size_t buffer_size) : I2SAudio()
{
  instance = this;
  wifi_channel_ = wifi_channel;
  this->buffer_size_ = buffer_size;
  this->buffer_ = (uint8_t *)malloc(MAX_ESP_NOW_PACKET_SIZE);
  memcpy(this->buffer_, transport_header, TRANSPORT_HEADER_SIZE);
}

void receiveCallback(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
  instance->on_receive(recv_info, data, len);
}

void EspNowTransport::on_receive(const esp_now_recv_info_t *recv_info, const uint8_t *data, size_t len) {
    uint8_t * mac_addr = recv_info->src_addr;
    //uint8_t * des_addr = recv_info->des_addr;

    if (mac_addr == NULL || data == NULL || len <= 0) {
        ESP_LOGE(TAG, "Receive cb arg error");
        return;
    }

  // first m_header_size bytes of m_buffer are the expected header
  if (valid_header(data, len))
  {
    xStreamBufferSend( receive_stream_, data + TRANSPORT_HEADER_SIZE, len - TRANSPORT_HEADER_SIZE, 1000);
  }
}

bool EspNowTransport::valid_header(const uint8_t *data, int len) {
  return ((len > TRANSPORT_HEADER_SIZE) && (len<=MAX_ESP_NOW_PACKET_SIZE) &&
          (memcmp(data, transport_header, TRANSPORT_HEADER_SIZE) == 0));
};

void EspNowTransport::start()
{
  this->receive_stream_ = xStreamBufferCreate( this->buffer_size_, 1 );
  // Set Wifi channel
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(this->wifi_channel_, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  esp_err_t result = esp_now_init();
  if (result == ESP_OK)
  {
    esp_now_register_recv_cb(receiveCallback);

    // this will broadcast a message to everyone in range
    esp_now_peer_info_t peerInfo = {};
    memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
    if (!esp_now_is_peer_exist(broadcastAddress))
    {
      esp_now_add_peer(&peerInfo);
    }
  }
}

size_t EspNowTransport::read(int16_t *samples, size_t count) {
  size_t size = xStreamBufferSpacesAvailable(this->receive_stream_);
  count = std::min(count, size);
  return 0;
}

size_t EspNowTransport::write(int16_t *samples, size_t count) {
 //  esp_now_send(broadcastAddress, this->buffer, m_index + TRANSPORT_HEADER_SIZE);
  return 0;
}
