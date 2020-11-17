#ifndef I2C_HEADER_FILE_H
#define	I2C_HEADER_FILE_H
#include <pic16f887.h>   
void i2c_init(void)
{
    SSPCON = 0b00101000;
    SSPCON2 = 0b00000000;
    SSPSTAT = 0b00000000;
    SSPADD = 0b00000100;
    //check status
}

void i2c_busIdle()
{
    while((SSPCON2 & 0b00011111) || (SSPSTAT & 0b00000100));    // Check if bus is idle
}

void i2c_start(void)         // Function to initiate start condition
{
SEN = 1;
i2c_busIdle();
}

void i2c_stop(void)          // Function to initiate stop condition
{
    PEN=1;
    i2c_busIdle();
}

void i2c_repeatedStart(void)    // Initiating repeated start condition
{
    RSEN=1;
    i2c_busIdle();
}

void ack(void)                 // Function to initiate acknowledge sequence 
{
    ACKDT = 0;
    ACKEN = 1;
    i2c_busIdle();
}

void Noack(void)              // Function to initiate No acknowledge sequence
{
    ACKDT = 1;
    ACKEN = 1;
    i2c_busIdle();
}

void receive_enable(void)      // Function to enable reception
{
    RCEN =1;
    i2c_busIdle();
}

void i2c_transmit()          // Function to transmit data
{
    i2c_start();    
    i2c_busIdle();
    
    SSPBUF = 0b10100010;           // 'Write' address of pcf8563     0x51 address 
    i2c_busIdle();
    SSPBUF = 0b00000000;           // Register address from where you want to start writing
    i2c_busIdle();
    SSPBUF = 0b00000000;           // control status reg1
    i2c_busIdle();
    SSPBUF = 0b00000000;           // control status reg2
    i2c_busIdle();
    SSPBUF = 0x00;           // Setting integrity and Seconds to zero
    i2c_busIdle();
    SSPBUF = 0x20;           // Setting Minutes to 15
    i2c_busIdle();
    SSPBUF = 0x22;           // Setting Hour to 10
    i2c_busIdle();
    SSPBUF = 0x24;           // Setting day to 4th 
    i2c_busIdle();
    SSPBUF = 0x00;           // Setting weekday to monday
    i2c_busIdle();
    SSPBUF = 0x08;           // Setting month to August
    i2c_busIdle();
    SSPBUF = 0x20;           // Setting year to 2020
    
    i2c_busIdle();
    i2c_stop();
}

int i2c_receive(void)        // Function to receive sec,min,... etc
{
  i2c_start();  
    
  SSPBUF = 0b10100010;              // Slave address + write
  i2c_busIdle();
  
  SSPBUF = 0x04;              // Write memory address from where you want to start reading 
  i2c_busIdle();
  
  i2c_repeatedStart();        // Repeated start condition
  i2c_start();                // start condition
    
  SSPBUF = 0b10100011;              // Slave address + read
  i2c_busIdle();
 
  
 
 receive_enable();            // Receive Hours
 int hour = SSPBUF;
 i2c_busIdle();
 Noack();

 i2c_stop();
 return hour;
}
#endif