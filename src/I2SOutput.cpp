
#include "I2SOutput.h"

#include <stdint.h>
#include <stdlib.h>

#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "esp_check.h"

#include "config.h"


void I2SOutput::start()
{
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &this->handle_, NULL));

    i2s_std_config_t tx_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED, // some codecs may require mclk signal, this example doesn't need it
            .bclk = I2S_SPEAKER_SERIAL_CLOCK,
            .ws = I2S_SPEAKER_LEFT_RIGHT_CLOCK,
            .dout = I2S_SPEAKER_SERIAL_DATA,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    tx_std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_RIGHT;
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(this->handle_, &tx_std_cfg));
}

size_t I2SOutput::write(int16_t *samples, size_t count)
{
    size_t w_bytes = count;

    /* (Optional) Preload the data before enabling the TX channel, so that the valid data can be transmitted immediately */
    while (w_bytes == count)
    {
        /* Here we load the target buffer repeatedly, until all the DMA buffers are preloaded */
        ESP_ERROR_CHECK(i2s_channel_preload_data(this->handle_, samples, count, &w_bytes));
    }

    /* Enable the TX channel */
    ESP_ERROR_CHECK(i2s_channel_enable(this->handle_));
    if (i2s_channel_write(this->handle_, samples, count, &w_bytes, 1000) != ESP_OK)
    {
        printf("I2S write failed\n");
    }
    return w_bytes;
}
