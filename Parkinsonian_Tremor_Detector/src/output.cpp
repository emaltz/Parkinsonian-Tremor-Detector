//output.cpp
#include "output.h"

bool previous_tremor = false;
bool disp_init = false;

//output the results of signal analysis to LCD
void display_output(){
  char mag_buffer[30];
  char freq_buffer[30];
  if((tremor_strength > 250) && (tremor_frequency > 2.5) && (previous_tremor == false)){
    previous_tremor = true;
    lcd.Clear(LCD_COLOR_RED);
    lcd.SetBackColor(LCD_COLOR_RED);
    sprintf(mag_buffer,"%d",(uint16_t)tremor_strength);
    sprintf(freq_buffer,"%d Hz",(uint8_t)tremor_frequency);
    lcd.DisplayStringAt(0, LINE(3),(uint8_t *) "TREMOR", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(4),(uint8_t *) "DETECTED", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(6),(uint8_t *) "Strength:", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(7),(uint8_t *) mag_buffer, CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(9),(uint8_t *) "Frequency:", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(10),(uint8_t *) freq_buffer, CENTER_MODE);
  }else if((tremor_strength > 250) && (tremor_frequency > 2.5) && (previous_tremor == true)){
    sprintf(mag_buffer,"%d",(uint16_t)tremor_strength);
    sprintf(freq_buffer,"%d Hz",(uint8_t)tremor_frequency);
    lcd.ClearStringLine(7);
    lcd.ClearStringLine(10);
    lcd.DisplayStringAt(0, LINE(7),(uint8_t *) mag_buffer, CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(10),(uint8_t *) freq_buffer, CENTER_MODE);
  }else if((tremor_strength == 0) && (tremor_frequency == 0) && (previous_tremor == false) && (disp_init == true)){
    lcd.DisplayStringAt(0, LINE(3),(uint8_t *) "No Tremor", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(4),(uint8_t *) "Detected", CENTER_MODE);
  }else if((tremor_strength == 0) && (tremor_frequency == 0) && (previous_tremor == true)){
    previous_tremor = false;
    lcd.Clear(LCD_COLOR_BLACK);
    lcd.SetBackColor(LCD_COLOR_BLACK);
    lcd.ClearStringLine(3);
    lcd.ClearStringLine(4);
    lcd.ClearStringLine(6);
    lcd.ClearStringLine(7);
    lcd.ClearStringLine(9);
    lcd.ClearStringLine(10);
    lcd.DisplayStringAt(0, LINE(3),(uint8_t *) "No Tremor", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(4),(uint8_t *) "Detected", CENTER_MODE);
  }
  if(disp_init == false){
    disp_init = true;
    lcd.ClearStringLine(3);
    lcd.DisplayStringAt(0, LINE(3),(uint8_t *) "No Tremor", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(4),(uint8_t *) "Detected", CENTER_MODE);
    printf("Disp Init: %d\n",disp_init);
  }
}

//serial communication function, was used for debugging throughout project
//i.e. sending signal, fft, timing data
void serial_comm(){
  //initialized to 01100111, g
  //char array is 40 elements, 40 bytes long
  char serial_buffer[N*2] = {};
  //quickly copy over data to buffer
  //iterate through all 40 bytes in sampled signal, 2 bytes taken each time, 20 iterations
  int k = 0;
  for(int i = 0;i<N;i++){
    serial_buffer[k] = (sampled_signal_y[i]>>8) & 0XFF;
    serial_buffer[k+1] = sampled_signal_y[i] & 0XFF;
    k+=2;
  }
  //this will be interrupted by sampler, write data to computer
  //40 iterations, 1 byte each time, 40 bytes
  for(int i = 0;i<N*2;i++){
    pc.write(&serial_buffer[i],1);
  }
  //send out max frequency reading from analysis, (truncated)
  // int16_t frequency_max = int16_t(freqBiny);
  // serial_buffer[0] = (frequency_max>>8) & 0XFF;
  // serial_buffer[1] = frequency_max & 0XFF;
  // pc.write(&serial_buffer[0],1);
  // pc.write(&serial_buffer[1],1);


  // send Fourier Transform Coefficients
  // int16_t magnitude_fourier[N] = {};
  // for(int i = 0;i<N;i++){
  //   magnitude_fourier[i] = abs(fft_data_y[i]);
  // }
  // k = 0;
  // for(int i = 0;i<N;i++){
  //   serial_buffer[k] = (magnitude_fourier[i]>>8) & 0XFF;
  //   serial_buffer[k+1] = magnitude_fourier[i] & 0XFF;
  //   k+=2;
  // }
  // for(int i = 0;i<N*2;i++){
  //   pc.write(&serial_buffer[i],1);
  // }

  // float timeread = serial_send_timer.read();
  // serial_send_timer.reset();
  // timeread = timeread * 1000000;
  // uint32_t intTimeVal = (uint32_t) timeread;
  // char time_sender[4];
  // uint8_t bit_shift = 24;
  // for(int i = 0;i<3;i++){
  //   time_sender[i] = (intTimeVal >>bit_shift) & 0xFF;
  //   bit_shift -= 8;
  // }
  // for(int i = 0;i<4;i++){
  //   pc.write(&time_sender[i],1);
  // }
}