//config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <mbed.h>
#include "drivers/LCD_DISCO_F429ZI.h"

//------------------------------Definitions---------------------------------

//sensitivity of 70 mdps/digit at scale 2000 dps
#define SCALING_FACTOR (0.07f)
#define PI 3.14159265358979323846

// Define gyroscope control registers and configuration settings
#define CTRL_REG1 0x20
// Output Data Rate = 200 Hz, Cutoff = 50, Normal Power, X,Y,Z Axis Enable
#define CTRL_REG1_CONFIG 0b01'10'1'1'1'1
#define CTRL_REG4 0x23
// Full Scale Selection = 2000 degrees per second
#define CTRL_REG4_CONFIG 0b0'0'10'0'00'0
#define FIFO_CTRL_REG 0x2E
// Put FIFO in stream mode, buffers data for approx 160 ms
#define FIFO_CTRL_REG_CONFIG 0b010'00000
// Define the address to read the X-axis lower data
#define OUT_X_L 0x28

// Define flags to be used for interrupts and tasks
#define SPI_FLAG 1
#define TAKE_SAMPLE_FLAG 2
#define PROCESS_DATA_FLAG 3

// Communication Definitions
static BufferedSerial pc(USBTX, USBRX); //Serial output to PC for debugging
// Flags object for task management
extern EventFlags flags;
// Initialize the SPI object with specific pins
extern SPI spi;
// Initialize Display
extern LCD_DISCO_F429ZI lcd;
// Buffers for sending and receiving data over SPI, accessed by multiple threads
extern uint8_t write_buf[32], read_buf[32];
//Threads include this sampling thread, and main loop thread
extern Thread sample_data_thread;
//Mutex for controlling access to shared resources
extern Mutex mutex;

//config SPI, serial communication, gyroscope settings, lcd
void config_communication();
void config_gyroscope();
void config_lcd();

//ISRs
void spi_complete_cb(int event);
void sampler_cb();
void processor_cb();

#endif // CONFIG_H