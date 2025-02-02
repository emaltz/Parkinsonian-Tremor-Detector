Project Demonstration:
https://www.youtube.com/watch?v=Ut0C-nwx9YI

Evan Maltz, Real Time Embedded Systems Challenge Spring 2024
The goal is to detect Parkinson's tremors, which occur at a rate from around 3 to 6 Hz. The structure of this program is divided into two threads.  One thread is the sampler, the other is a chain of signal processing and analysis.  Data is sampled from the peripheral gyroscope at a rate of 32 Hz, stored in a circular buffer which is accessed by both the sampler and processor. Signal data is processed by a moving average low-pass filter to reduce the sensitivity of the gyroscope output, transformed into the frequency domain using an implementation of the fast Fourier transform, and then analyzed to determine if a tremor is present or not. Implemented using MBED OS on an STM32F429ZI microcontroller, interfaced with the ST I3G4250D MEMS gyroscope, prototyped on the discovery dev board.
