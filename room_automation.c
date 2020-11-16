#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pic16f887.h>
#include "Config_Hardware.h"
#include "Usart_Config.h"
#define RB0 door_outer_IR
#define RB1 door_inner_IR
#define RB2 bathroom_door_IR



#define _XTAL_FREQ 20000000
#define FOSC 20000000L                      // Using External crystal of 20 MHz

unsigned char person_enter_room,person_exit_room,bathroom_light,count=0,used_inner=0,used_outer=0,inner_used_already=0,outer_used_already=0;


void USART_Init(long baud_rate)
{
    TRISC6 = 0;                     /* Make Tx pin as output */
    TRISC7 = 1;                     /* Make Rx pin as input */
    SPBRG = (int)BAUDRATE;          /* Baud rate, SPBRG = (F_CPU /(64*baud_rate))-1 */
 
    
    TXSTA = 0x20;   //0010 0000               /* Transmit Enable(TX) enable */ 
    RCSTA = 0x90;   // 1001 0000              /* Receive Enable(RX) enable and serial port enable */
}
/******************TRANSMIT FUNCTION*****************************************/ 
void USART_TxChar(char data)
{        
    while(!TXIF)                   /* Wait for buffer empty */
    TXREG = data;                   /* Copy data to be transmit in Tx register */
}
/*******************RECEIVE FUNCTION*****************************************/
char USART_RxChar()
{
    while(!RCIF);                   /* Wait for receive interrupt flag*/
    return(RCREG);                  /* Return received data */
}

/*******************SEND STRING FUNCTION*****************************************/
void USART_SendString(const char *str)
{
   while(*str!='\0')                /* Transmit data until null */
   {            
        USART_TxChar(*str);
        str++;
   }
}


void interrupt ISR()
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
    TRISC=0b00000000;
    TRISD=0b00000000;
    TRISE=0b00000000;

    PORTA=0b00000000;
    PORTB=0b00000111;
    PORTC=0b00000000;
    PORTD=0b00000000;
    PORTE=0b00000000;
    
    PR2=0xF9;                   // Period is 200ms
    T2CON=0b00000001;
    CCP1CON=0b00001100;         
    
    INTCON=0b11001000;
    
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

        if(bathroom_light=1)
        {
            bathroom_light=0;
            
        }
    }
}
