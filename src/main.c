#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"

#include "I2SInput.h"
#include "I2SOutput.h"
#include "EspNowTransport.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "config.h"

static const char *TAG = "combadge";

#define  CONFIG_BLINK_GPIO

static uint8_t s_led_state = 0;
static led_strip_handle_t led_strip;

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        led_strip_set_pixel(led_strip, 0, 16, 16, 16);
        /* Refresh the strip to send data */
        led_strip_refresh(led_strip);
    } else {
        /* Set all LED off to clear all pixels */
        led_strip_clear(led_strip);
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = GPIO_BLINK_LED,
        .max_leds = 1, // at least one LED on board
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}


bool digitalRead()
{
  return false;
}
void set_speaker(bool enable)
{
  if (I2S_SPEAKER_SD_PIN != -1)
  {
    // digitalWrite(I2S_SPEAKER_SD_PIN, enable ? HIGH : LOW);
  }
}

I2SAudio *output_;
I2SAudio *input_;
I2SAudio *transport_;

void app_main()
{
  input_ = new I2SInput();
  output_ = new I2SOutput();

  transport_ = new EspNowTransport(ESP_NOW_WIFI_CHANNEL, 8192);
  transport_->start();

  if (I2S_SPEAKER_SD_PIN != -1)
  {
    // pinMode(I2S_SPEAKER_SD_PIN, OUTPUT);
    gpio_reset_pin(I2S_SPEAKER_SD_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(I2S_SPEAKER_SD_PIN, GPIO_MODE_OUTPUT);
  }
  gpio_reset_pin(GPIO_TRANSMIT_BUTTON);
  gpio_set_direction(GPIO_TRANSMIT_BUTTON, GPIO_MODE_OUTPUT);
  gpio_set_pull_mode(GPIO_TRANSMIT_BUTTON,  GPIO_PULLDOWN_ONLY);
  // pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLDOWN);

  int16_t *r_buf = (int16_t *)calloc(1, BUFF_SIZE);
  assert(r_buf);
  size_t samples_read = -0;
  printf("Start Combade now.");
  samples_read = input_->read(r_buf, 128);
  // continue forever
  while (true)
  {
    // do we need to start transmitting?
    if (!digitalRead())
    {
      output_->stop();
      input_->start();
      while (!digitalRead())
      {
        samples_read = input_->read(r_buf, 128);
        transport_->write(r_buf, samples_read);
      }
      transport_->flush();
      input_->stop();
    }
    if (!transport_->is_empty())
    {
      output_->start();
      set_speaker(true);
      samples_read = transport_->read(r_buf, BUFF_SIZE);
      output_->write(r_buf, samples_read);
      if (transport_->is_empty())
      {
        set_speaker(false);
        output_->stop();
      }
    }
  }
  free(r_buf);
  vTaskDelete(NULL);
}
