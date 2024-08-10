#pragma once
#include "driver/i2s_std.h"

/**
 * Base Class for both the ADC and I2S sampler
 **/
class I2SAudio
{

public:
    ~I2SAudio()
    {
        this->stop();
    }

    virtual void start() = 0;
    virtual void stop()
    {
        if (this->handle_ != nullptr)
        {

            /* Have to stop the channel before deleting it */
            i2s_channel_disable(this->handle_);
            /* If the handle is not needed any more, delete it to release the channel resources */
            i2s_del_channel(this->handle_);
            this->handle_ = nullptr;
        }
    }
    virtual size_t read(int16_t *samples, size_t count) { return 0; }
    virtual size_t write(int16_t *samples, size_t count) { return 0; }

    virtual bool is_empty() { return true; }
    virtual bool is_full() { return false; }

    virtual void flush() {};

protected:
    i2s_chan_handle_t handle_ = nullptr;
};
