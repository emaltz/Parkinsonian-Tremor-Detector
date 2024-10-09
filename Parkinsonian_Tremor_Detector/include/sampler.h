//sampler.h
#ifndef SAMPLER_H
#define SAMPLER_H

#include "signal_processing.h"

class Circular_Buffer;
extern Timer sampling_timer;
extern Circular_Buffer circ_buffer;

class Circular_Buffer{
    public:
    int16_t buffer_x[32] = {0};
    int16_t buffer_y[32] = {0};
    int16_t buffer_z[32] = {0};

    void init_buffer(int16_t sampx, int16_t sampy, int16_t samp_z, uint8_t *sample_iteration);
    void shift_buffer(int16_t sampx, int16_t sampy, int16_t sampz);
    void copy_buffer();
};

//sample and store data from the gyroscope
void sample_data();

#endif //SAMPLER_H