
/*===================================CPEG222====================================
 * Program:		Project_1
 * Authors: 	Luke Fanizzi and Phil (Zhuonan) Wang
 * Date: 		9/12/2019
 * Description: Our project uses on-board BTNs to control the lighting sequence of
 * the on-board LEDs 0 thru 7.  The initial state is LD0 through LD7 ON.
 * The program waits for a BTN press with the following results:
 * -BTNC turns all the LEDS either on or off from LD0 - LD7. This is done from left to 
 * right if the switch is off and right to left if the switch is on.
 * 
 * Inputs: Button press and switch flip
 * Output: LED is turned on or off one at a time until they are all on or off.
==============================================================================*/
/*------------------ Board system settings. PLEASE DO NOT MODIFY THIS PART ----------*/
#ifndef _SUPPRESS_PLIB_WARNING          //suppress the plib warning during compiling
    #define _SUPPRESS_PLIB_WARNING      
#endif
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config FPBDIV = DIV_8           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
/*----------------------------------------------------------------------------*/
     
#include <xc.h>   //Microchip XC processor header which links to the PIC32MX370512L header
//#include "config.h" // Basys MX3 configuration header
//#include "led.h"    // Digilent Library for using the on-board LEDs
//#include "btn.h"    // Digilent Library for using the on-board BTNs
#//include "swt.h"    // Switches

/* --------------------------- Forward Declarations-------------------------- */
void delay_ms(int ms);

/* -------------------------- Definitions------------------------------------ */
#define SYS_FREQ    (80000000L) // 80MHz system clock
#define tris_LEDS_GRP   TRISA
#define  lat_LEDS_GRP   LATA
#define latinv_LEDS_GRP LATAINV
#define  prt_LEDS_GRP   PORTA
#define  msk_LEDS_GRP   0xFF    
#define  lat_LEDS_GRP_ADDR   0xBF886030
#define macro_disable_interrupts __builtin_disable_interrupts()

//#define btnd PortAbits.RA15
#define btnc PORTFbits.RFO
#define led0 LATAbits.LATA0
#define led1 LATAbits.LATA1
#define led2 LATAbits.LATA2
#define led3 LATAbits.LATA3
#define led4 LATAbits.LATA4
#define led5 LATAbits.LATA5
#define led6 LATAbits.LATA6
#define led7 LATAbits.LATA7
ANSELA = 0xFFFF
TRISA = 0x0000
//TRISF = 0xFFFF;
TRISA |= 0 x 80
#define SW0 TRISF_SWT_SWT0

//TRISA |= 0x80;
//if(btnd)
//{}
int LD_position = 0x01;// Initially turn on LD0

int buttonLock = 0;             // Variable to "lock" the button
int delay = 100;  // Variable to set delay to 100 ms
//int delay2 = 700;
/* ----------------------------- Main --------------------------------------- */

int main(void) 
{
    DDPCONbits.JTAGEN = 0;      // Statement is required to use Pin RA0 as IO
    //LED_Init();         //	This function initializes the hardware involved in the LED module. 
                        //  The pins corresponding to LEDs are initialized as digital outputs.
    //BTN_Init();         //  This function initializes the hardware involved in the BTN module.
    //SWT_Init();         // Initializes switches
    //LED_SetGroupValue(LD_position); 
    
    // sets all LEDS to on initially
    led0 = 1;
    led1 = 1;
    led2 = 1;
    led3 = 1;
    led4 = 1;
    led5 = 1;
    led6 = 1;
    led7 = 1;
    int count = 1; // creates counter
    while (1)  // while true loop that allows the program to loop through back to mode 1
    {
/*-------------------- Main logic and actions start --------------------------*/
        if(buttonLock && !btnc) // Check button status and in this case upper button(BTNU)) is used.
                                             // Please check the btn.c if you want to use other button
        {
            delay_ms(delay);        // wait 100ms or you can change the value to
                                    // make it more sensitive to the button by changing the variable delay
            buttonLock = 0; //unlock buttons if all off
        }
        
        if(btnc && !buttonLock)// Actions when upper button is pressed
        {
            count += 1;
            delay_ms(delay);        // wait 100ms or you can change the value to
            // make it more sensitive to the button by changing the variable delay
            buttonLock = 1;
            
            if (count == 1){
                
                //LED_SetGroupValue(1);
                led0 = 1;
                led1 = 1;
                led2 = 1;
                led3 = 1;
                led4 = 1;
                led5 = 1;
                led6 = 1;
                led7 = 1;
                
            }
            
            
            if (count == 2 && !SWT_GetValue(0)){     // and switch off
             led7 = 0; //turn off LEDS from left to right
             delay_ms(delay + 600);
             led6 = 1;
             delay_ms(delay + 600);
             led5 = 1;
             delay_ms(delay + 600);
             led4 = 1;
             delay_ms(delay + 600);
             led3 = 1;
             delay_ms(delay + 600);
             led2 = 1;
             delay_ms(delay + 600);
             led1 = 1;
             delay_ms(delay+600);
             led0 = 1;
             delay_ms(delay+600);
             count +=1;
            }
             
            else if (count == 2 && SWT_GetValue(0) ){// and switch on
             led0 = 0; //turn off LEDS from right to left
             delay_ms(delay + 600);
             led1 = 0;
             delay_ms(delay + 600);
             led2 = 0;
             delay_ms(delay + 600);
             led3 = 0;
             delay_ms(delay + 600);
             led4 = 0;
             delay_ms(delay + 600);
             led5 = 0;
             delay_ms(delay + 600);
             led6 = 0;
             delay_ms(delay+600);
             led7 = 0;
             delay_ms(delay+600);
             count+=1;
            }
            
            if (count == 3){
                 //LED_SetGroupValue(0);
                led0 = 0;
                led1 = 0;
                led2 = 0;
                led3 = 0;
                led4 = 0;
                led5 = 0;
                led6 = 0;
                led7 = 0;
             }
            
            if (count == 4 && SWT_GetValue(0)){// and switch on
             led0 = 1; //Turn on LEDS from right to left
             delay_ms(delay + 600);
             led1 = 1;
             delay_ms(delay + 600);
             led2 = 1;
             delay_ms(delay + 600);
             led3 = 1;
             delay_ms(delay + 600);
             led4 = 1;
             delay_ms(delay + 600);
             led5 = 1;
             delay_ms(delay + 600);
             led6 = 1;
             delay_ms(delay+600);
             led7 = 1;
             delay_ms(delay+600);
             
                 count = 1;
            
            }
             
            else if (count == 4 && !SWT_GetValue(0)){// and switch off
             led7 = 1; //Turn on LEDS from right to left
             delay_ms(delay + 600);
             led6 = 1;
             delay_ms(delay + 600);
             led5 = 1;
             delay_ms(delay + 600);
             led4 = 1;
             delay_ms(delay + 600);
             led3 = 1;
             delay_ms(delay + 600);
             led2 = 1;
             delay_ms(delay + 600);
             led1 = 1;
             delay_ms(delay+600);
             led0 = 1;
             delay_ms(delay+600);
             count = 1;
            }
             
                 
                                   
                //LD_position = LD_position << 1;// Turn on left LD and turn off current one
                //LEDS_Led1SetValue(1);
                //delay_ms(delay);
                
            //if (LD_position>128)// if past LD7 set back top LD0
                //LD_position = 0x01;
            
            //LED_SetGroupValue(LD_position); //sets the value for all 8 LEDs
            
        }
    }
/*--------------------- Action and logic end ---------------------------------*/
}
/* ----------------------------------------------------------------------------- 
**	delay_ms
**	Parameters:
**		ms - amount of milliseconds to delay (based on 80 MHz SSCLK)
**	Return Value:
**		none
**	Description:
**		Create a delay by counting up to counter variable
** -------------------------------------------------------------------------- */
void delay_ms(int ms){
	int		i,counter;
	for (counter=0; counter<ms; counter++){
        for(i=0;i<1426;i++){}   //software delay 1 milli-sec
    }
}




