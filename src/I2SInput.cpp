#include <stdint.h>
#include <stdlib.h>

#include "I2SInput.h"
#include "soc/soc_caps.h"
#include "driver/i2s_pdm.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_check.h"
#include "config.h"

void I2SInput::start()
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &this->handle_));

    i2s_pdm_rx_config_t pdm_rx_cfg = {
        .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
            .slot_cfg = {
            .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
            .slot_mode = I2S_SLOT_MODE_MONO,
            .slot_mask = I2S_PDM_SLOT_RIGHT,
        },
        .gpio_cfg = {
            .clk = I2S_MIC_SERIAL_CLOCK,
            .din = I2S_MIC_SERIAL_DATA,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_pdm_rx_mode(this->handle_, &pdm_rx_cfg));
}

size_t I2SInput::read(int16_t *samples, size_t count)
{
    // read from i2s

    assert(this->handle_);
    size_t bytes_read = 0;

    ESP_ERROR_CHECK(i2s_channel_enable(this->handle_));

    if (i2s_channel_read(this->handle_, samples, count, &bytes_read, 1000) == ESP_OK)
    {
        printf("Read:(%d) %02x %02x %02x %02x  %02x %02x %02x %02x\n", bytes_read,
               samples[0], samples[1], samples[2], samples[3], samples[4], samples[5], samples[6], samples[7]);
    }
    else
    {
        printf("I2S read failed\n");
    }

    return bytes_read;
}
