#pragma once

#include "I2SAudio.h"

class I2SInput : public I2SAudio
{
public:
    size_t read(int16_t *samples, size_t count) override;
    void start() override;
};
