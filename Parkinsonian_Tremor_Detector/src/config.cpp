//config.cpp
#include "config.h"

// Communication Definitions
// Flags object for task management
EventFlags flags;
//SPI Object
SPI spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel);
//Display Object
LCD_DISCO_F429ZI lcd;
//Buffers for sending and receiving data over SPI
uint8_t write_buf[32], read_buf[32];
//Thread for sampling
Thread sample_data_thread(osPriorityHigh,OS_STACK_SIZE);
//Mutex for controlling access to shared resources (circular buffer)
Mutex mutex;

//Interrupt Service Routines
void spi_complete_cb(int event){
  flags.set(SPI_FLAG);
}
//Attached to the sampling ticker. Occurs at a rate of 32 Hz
void sampler_cb(){
  flags.set(TAKE_SAMPLE_FLAG);
}

//configure SPI and Sercom
void config_communication(){
  pc.set_baud(9600);
  pc.set_format(8,BufferedSerial::None,1);
  spi.format(8, 3);
  spi.frequency(1'000'000);
}

//configure on-board gyroscope
void config_gyroscope(){
  // Configure CTRL_REG1 register
  write_buf[0] = CTRL_REG1;
  write_buf[1] = CTRL_REG1_CONFIG;

  spi.transfer(write_buf, 2, read_buf, 2, spi_complete_cb);
  flags.wait_all(SPI_FLAG);

  // Configure CTRL_REG4 register
  write_buf[0] = CTRL_REG4;
  write_buf[1] = CTRL_REG4_CONFIG;

  spi.transfer(write_buf, 2, read_buf, 2, spi_complete_cb);
  flags.wait_all(SPI_FLAG);

  // Configure FIFO_CTRL_REG register
  write_buf[0] = FIFO_CTRL_REG;
  write_buf[1] = FIFO_CTRL_REG_CONFIG;

  spi.transfer(write_buf, 2, read_buf, 2, spi_complete_cb);
  flags.wait_all(SPI_FLAG);

  // Dummy
  write_buf[1] = 0xFF;
}

//configure LCD
void config_lcd(){
  lcd.Clear(LCD_COLOR_BLACK);
  lcd.SetBackColor(LCD_COLOR_BLACK);
  lcd.SetTextColor(LCD_COLOR_WHITE);
  lcd.SetFont(&Font24);
  lcd.DisplayStringAt(0, LINE(3),(uint8_t *) "Booting Up...", CENTER_MODE);
}