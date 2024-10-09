//main.cpp

//Evan Maltz, eem398@nyu.edu
//Real Time Embedded Systems
//Embedded Challenge: Parkinson's Tremor Detector

//Project Description:
/* The goal is to detect Parkinson's tremors, which occur at a rate from around 3 to 6 Hz.
The structure of this program is divided into two threads. 
One thread is the sampler, the other is a chain of signal processing and analysis. 
Data is sampled from the on-board gyroscope's FIFO buffer at a rate of 32 Hz, stored in a circular buffer which is
accessed by both the sampler and processor. The processor checks the circular buffer every 125 ms, or at a rate of 8 Hz.
Signal data is processed by a moving average low-pass filter to reduce the sensitivity of the gyroscope output, 
transformed into the frequency domain using an implementation of the fast fourier 
transform, and then analyzed to determine if a tremor is present or not.
Implemented using MBED on an STM32F429ZI microcontroller, interfaced with the ST I3G4250D MEMS gyroscope, 
prototyped on the discovery dev board.*/

#include "sampler.h"
#include "signal_processing.h"
#include "output.h"
#include <stdio.h>
using namespace std;

//Ticker to maintain sampling rate of 32 Hz
Ticker sampler;

int main() {
  //accessed by filter, fft, analyzer
  complex <double> x_data[N] = {0};
  complex <double> y_data[N] = {0};
  complex <double> z_data[N] = {0};
  //initialize SPI, serial port, gyroscope, lcd
  config_communication();
  config_gyroscope();
  config_lcd();

  //initialize sampling at a rate of 32 Hz
  flags.set(TAKE_SAMPLE_FLAG);
  sample_data_thread.start(sample_data);
  sampler.attach(&sampler_cb,0.03125f);

  while(true) {
    ThisThread::sleep_for(40);
    //accesses the shared circular buffer every 125 ms and processes data, outputs results to LCD
    flags.wait_all(PROCESS_DATA_FLAG);
    flags.clear(PROCESS_DATA_FLAG);
    mutex.lock();
    circ_buffer.copy_buffer();
    mutex.unlock();
    //smooth raw data through a moving avg filter
    filter_data(x_data, y_data, z_data);
    //transform data into the frequency domain
    fft(x_data, N);
    fft(y_data, N);
    fft(z_data, N);
    //determine if a tremor is present
    analyze_signal(x_data, y_data, z_data);
    //output results to display
    display_output();
    }
}