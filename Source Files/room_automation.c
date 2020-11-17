#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pic16f887.h>
#include "I2C.h"
#include "Config_Hardware.h"
#include "Usart_Config.h"
#define door_outer_IR RB0
#define door_inner_IR RB1
#define bathroom_door_IR RB2



#define _XTAL_FREQ 20000000
#define FOSC 20000000L                      // Using External crystal of 20 MHz

unsigned char person_enter_room,person_exit_room,bathroom_light,count=0,used_inner=0,used_outer=0,inner_used_already=0,outer_used_already=0,hour;


//Interrupts
void __interrupt () my_isr_routine (void)
{
    if(RBIF==1)
    {
        RBIF=0;
        if(used_outer=1 && door_outer_IR==0)  {outer_used_already=1; used_outer=0; door_outer_IR=1;}
        if(used_outer=1 && door_inner_IR==0)  {used_outer=0; door_inner_IR=1;}
        
        if(used_inner=1 && door_inner_IR==0) {inner_used_already=1; used_inner=0; door_inner_IR=1;}
        if(used_inner=1 && door_outer_IR==0)  {used_inner=0; door_outer_IR=1;}

        if (door_outer_IR==0 && used_outer==0 && used_inner==0)       
        {
            door_outer_IR=1;       
            person_enter_room=1;
        }
        if (door_inner_IR==0 && used_outer==0 && used_inner==0)       
        {
            door_inner_IR=1;       
            person_exit_room=1;
        }
        if (bathroom_door_IR==0)    
        {
            bathroom_door_IR=1;    
            bathroom_light=1;
        }
    }
}

void main()
{
    TRISA=0b00000000;
    TRISB=0b00000111;
    TRISC=0b00011000;
    TRISD=0b00000000;
    TRISE=0b00000000;

    PORTA=0b00000000;
    PORTB=0b00000111;
    PORTC=0b00000000;
    PORTD=0b00000000;
    PORTE=0b00000000;
    
    USART_Init(9600);
    
    PR2=0xF9;                   // Period is 200ms
    T2CON=0b00000001;
    CCP1CON=0b00001100;         
    
    INTCON=0b11001000;
    i2c_init();
    i2c_transmit();
    
    while(1)
    {
        if (person_enter_room==1)
        {
            used_outer=1;
            person_enter_room=0;
            count++;
            if (count==1)
            {
               USART_Sendstring("light_on"); 
            }
        }
        if (person_exit_room==1)
        {
            used_inner=1;
            person_exit_room=0;
            count--;
            if (count==0)
            {
               USART_Sendstring("light_off"); 
            }
        }
        if (outer_used_already==1)
        {
            outer_used_already=0;
            count--;
            if (count==0)
            {
               USART_Sendstring("light_off"); 
            }
        }
        if (inner_used_already==1)
        {
            inner_used_already=0;
            count++;
            if (count==1)
            {
               USART_Sendstring("light_on"); 
            }
        }

        if(bathroom_light==1)
        {   
            bathroom_light=0;
            hour=i2c_receive();
            if (hour>0 && hour<6)
            {
                CCPR1L=0b01111101;
            }
            else
            {
                CCPR1L=0b11111010;
            }
            RC2=!RC2;            
        }
    }
}




