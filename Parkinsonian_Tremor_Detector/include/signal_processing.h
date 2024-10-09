#ifndef SIGNAL_PROCESSING_H
#define SIGNAL_PROCESSING_H

#include "config.h"
#include "sampler.h"
#include <cmath>
#include <complex>

//Global Variables Shared Between Sampling Thread, Main Thread
extern const int N;
extern uint16_t Fs;
//buffers for the processor
extern int16_t sampled_signal_x[32];
extern int16_t sampled_signal_y[32];
extern int16_t sampled_signal_z[32];
//track how many samples have been collected
extern uint16_t sampled;

//debug variables
// extern uint16_t sampling_rounds;
// extern uint16_t processed;

//accessed by analyzer, output
extern float tremor_strength;
extern float tremor_frequency;

//put the samples through a 3 pt moving average low pass filter. store for fft.
void filter_data(complex <double> *x,complex <double> *y,complex <double> *z);
//perform fft, function is called for each axis x, y, z
void fft(complex <double> *x,int n);
//analyze data and determine if a tremor is present
void analyze_signal(complex <double> *x, complex <double> *y, complex <double> *z);

#endif //SIGNAL_PROCESSING_H