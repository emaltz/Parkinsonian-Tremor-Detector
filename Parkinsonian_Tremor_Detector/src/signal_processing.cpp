//signal_processing.cpp
#include "signal_processing.h"

//Global Variables Shared Between Sampling Thread, Main Thread
const int N = 32;
uint16_t Fs = 32;
int16_t sampled_signal_x[32] = {0};
int16_t sampled_signal_y[32] = {0};
int16_t sampled_signal_z[32] = {0};
uint16_t sampled = 0;

//debug variables
// uint16_t processed = 0;
// uint16_t sampling_rounds = 0;

//accessed by analyzer, output
float tremor_strength = 0;
float tremor_frequency = 0;

//three point moving average low pass filter to smooth data
void filter_data(complex <double> *x, complex <double> *y, complex <double> *z){
  for(int k = 0;k<N;k++){
    if(k == 0 || k == 1){
      x[k] = sampled_signal_x[k];
      y[k] = sampled_signal_y[k];
      z[k] = sampled_signal_z[k];
    }
    else{
      x[k] = (sampled_signal_x[k] + sampled_signal_x[k-1] + sampled_signal_x[k-2])/3;
      y[k] = (sampled_signal_y[k] + sampled_signal_y[k-1] + sampled_signal_y[k-2])/3;
      z[k] = (sampled_signal_z[k] + sampled_signal_z[k-1] + sampled_signal_z[k-2])/3;
    }
  }
}

//implementation of Cooley-Tukey FFT algorithm
void fft(complex <double> *x,int n){
  if(n <= 1) return;
  //Divide signal into odd and even parts
  //n will never be bigger than 32
  complex <double> even[n/2],odd[n/2];
  for(int i = 0;i < n/2;i++){
    even[i] = x[2*i];
    odd[i] = x[2*i + 1];
  }
  // Recursively call fft on smaller length signal parts
  fft(even,n/2);
  fft(odd,n/2);
  complex <double> j(0, 1);
  for(int k = 0;k < n/2;k++){
    //twiddle factors
    complex <double> t = exp(-j * (2 * PI * k / n)) * odd[k];
    x[k] = even[k] + t;
    x[k + n/2] = even[k] - t;
  }
}

//analyze data to see if a tremor is present
void analyze_signal(complex <double> *x, complex <double> *y, complex <double> *z){
    //variables for analysis and comparison
    float magnitudex = 0;
    float magnitudey = 0;
    float magnitudez = 0;
    float freqBinTemp = 0;
    //find the maximum magnitude with corresponding frequency in each axis
    float maxMagx = 0;
    float freqBinx = 0;
    float maxMagy = 0;
    float freqBiny = 0;
    float maxMagz = 0;
    float freqBinz = 0;
    //shift fft values to isolate frequencies of interest
    for(int k = 0;k<N/2;k++){
      x[k+(N/2)] = x[k];
      y[k+(N/2)] = y[k];
      z[k+(N/2)] = z[k];
    }
    //cycle through frequencies of spectrum
    for(int k = 0;k<N;k++){
      //calculate frequency of each k bin in Hz
      freqBinTemp = Fs * k / N - Fs / 2;
      //if frequencies are 0 or positive, proceed
      if(freqBinTemp >= 0){
        //find the maximum magnitude in each axis
        magnitudex = abs(x[k]);
        magnitudey = abs(y[k]);
        magnitudez = abs(z[k]);
        if(maxMagx < magnitudex){
            maxMagx = magnitudex;
            freqBinx = freqBinTemp;
        }
        if(maxMagy < magnitudey){
            maxMagy = magnitudey;
            freqBiny = freqBinTemp;
        }
        if(maxMagz < magnitudez){
            maxMagz = magnitudez;
            freqBinz = freqBinTemp;
        }
      }
    }
    //cycle through calculated max magnitudes and frequencies for each axis, choose output tremor detected
    //amplitude threshold of 250 for tremor detection
    if((maxMagx > 250) && (freqBinx >= 2.5) || (maxMagy > 250) && (freqBiny >= 2.5) || (maxMagz > 250) && (freqBinz >= 2.5)){
      if((maxMagx > maxMagy) && (maxMagx > maxMagz)){
        tremor_strength = maxMagx;
        tremor_frequency = freqBinx;
      }
      if((maxMagy > maxMagx) && (maxMagy > maxMagz)){
        tremor_strength = maxMagy;
        tremor_frequency = freqBiny;
      }
      if((maxMagz > maxMagx) && (maxMagz > maxMagy)){
        tremor_strength = maxMagz;
        tremor_frequency = freqBinz;
      }
    } else{
      tremor_strength = 0;
      tremor_frequency = 0;
    }
}