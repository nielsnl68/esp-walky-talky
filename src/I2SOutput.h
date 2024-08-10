#pragma once

#include "I2sAudio.h"

/**
 * Base Class for both the ADC and I2S sampler
 **/
class I2SOutput : public I2SAudio
{
public:
    void start() override;
    size_t write(int16_t *samples, size_t count) override;
};
