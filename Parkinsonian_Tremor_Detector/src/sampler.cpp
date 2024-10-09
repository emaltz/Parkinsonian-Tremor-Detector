//sampler.cpp
#include "sampler.h"

Timer sampling_timer;
Circular_Buffer circ_buffer;

//sampling thread function. takes samples at 32 Hz, stores in a circular buffer, calls on processor every 125 ms
void sample_data(){
  uint8_t sample_iteration = 0;
  int16_t signed_gx,signed_gy,signed_gz;

  while(true){
    //Wait to take a sample using the ticker
    //ThisThread::sleep_for(5);
    flags.wait_all(TAKE_SAMPLE_FLAG);
    //Prepare to read the gyroscope values starting from OUT_X_L, grab the Y and Z vals
    write_buf[0] = OUT_X_L | 0x80 | 0x40;
    //Perform the SPI transfer to read 6 bytes of data (for x, y, and z axes)
    spi.transfer(write_buf, 7, read_buf, 7, spi_complete_cb);
    flags.wait_all(SPI_FLAG);
    flags.clear(SPI_FLAG);
    //Convert data into signed integers and scale by sensitivity factor
    signed_gx = int16_t((((uint16_t)read_buf[2]) << 8) | ((uint16_t) read_buf[1])) * SCALING_FACTOR;
    signed_gy = int16_t((((uint16_t)read_buf[4]) << 8) | ((uint16_t) read_buf[3])) * SCALING_FACTOR;
    signed_gz = int16_t((((uint16_t)read_buf[6]) << 8) | ((uint16_t) read_buf[5])) * SCALING_FACTOR;

    //store samples in the shared circular buffer, set a flag once buffer is init
    mutex.lock();
    if (sample_iteration < N){
        //if the buffer isn't init, fill it up
        circ_buffer.init_buffer(signed_gx,signed_gy,signed_gz,&sample_iteration);
    }else{
        circ_buffer.shift_buffer(signed_gx,signed_gy,signed_gz);
    }
    mutex.unlock(); 
    flags.clear(TAKE_SAMPLE_FLAG);
    sampled++;
    //process data every 125 ms
    if(sampled == (uint8_t)(N/8)){
        flags.set(PROCESS_DATA_FLAG);
        sampled = 0;
    }
    ThisThread::sleep_for(25);
  }
}

//circular buffer functions
void Circular_Buffer::init_buffer(int16_t sampx, int16_t sampy, int16_t sampz, uint8_t *sample_iteration){
    //assign the prepared samples to their place in the circular buffer
    buffer_x[*sample_iteration] = sampx;
    buffer_y[*sample_iteration] = sampy;
    buffer_z[*sample_iteration] = sampz;
    (*sample_iteration)++;
}

void Circular_Buffer::shift_buffer(int16_t sampx, int16_t sampy, int16_t sampz){
    //shift all samples down by 1
    for(int k = 0;k<(N-1);k++){
        buffer_x[k] = buffer_x[k + 1];
        buffer_y[k] = buffer_y[k + 1];
        buffer_z[k] = buffer_z[k + 1];
    }
    //add the latest sample
    buffer_x[N-1] = sampx;
    buffer_y[N-1] = sampy;
    buffer_z[N-1] = sampz;
}

void Circular_Buffer::copy_buffer(){
    for(int k = 0;k<N;k++){
        sampled_signal_x[k] = buffer_x[k];
        sampled_signal_y[k] = buffer_y[k];
        sampled_signal_z[k] = buffer_z[k];
    }
}