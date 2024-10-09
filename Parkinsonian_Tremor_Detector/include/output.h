//output.h
#ifndef OUTPUT_H
#define OUTPUT_H

#include "signal_processing.h"

extern bool previous_tremor;
extern bool disp_init;

//output the result to the lcd of the microcontroller
void display_output();
void serial_comm();

#endif //OUTPUT_H