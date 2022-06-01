/*===================================CPEG222=================================
 * Program:        CPEG222_project_2_template
 * Authors:     Chengmo Yang (Template), Luke Fanizzi (main project code)
 * Date:         9/13/2019
 * Description: This project 2 template uses on-board SSD to implement a counter (0-9999).
 * The speed of the counter can be adjusted and the when the SSD display 9999,
 * it will come back to 0 automatically.
 =============================================================================*/
/*------------------------ System setting part please DO NOT CHANGE ---------*/
#ifndef _SUPPRESS_PLIB_WARNING          //suppress the plib warning during compiling
#define _SUPPRESS_PLIB_WARNING
#endif

#include <xc.h>   //Microchip XC processor header which links to the PIC32MX370512L header
//#include "config.h" // Basys MX3 configuration header
//#include "ssd.h"
//#include "ssd.c"

#pragma config OSCIOFNC =	ON
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config FPBDIV = DIV_8           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
/*---------------------------------------------------------------------------*/
/* ---------------------------Forward Declarations-------------------------- */
void delay_ms(int ms);
void toggle_display(int* display, int* decimal);
/* --------------------------Definitions------------------------------------ */
#define SYS_FREQ    (80000000L) // 80MHz system clock
int counter = 0; // a 4-digit counter keeping track of value to display (0~9999)
int mode = 1;
int REFRESH_RATE = 1;
int j = 0;
int place;
//Buttons
#define btnc PORTFbits.RF0
#define btnl PORTBbits.RB0
#define btnr PORTBbits.RB8
#define btnd PORTAbits.RA15 
#define btnu PORTBbits.RB1
//Display selection
#define AN0 LATBbits.LATB12
#define AN1 LATBbits.LATB13
#define AN2 LATAbits.LATA9
#define AN3 LATAbits.LATA10
//Segments
#define SegCA LATGbits.LATG12
#define SegCB LATAbits.LATA14
#define SegCC LATDbits.LATD6
#define SegCD LATGbits.LATG13
#define SegCE LATGbits.LATG15
#define SegCF LATDbits.LATD7
#define SegCG LATDbits.LATD7
#define SegDF LATGbits.LATG14

//output config
#define TRISB12 bits.TRISB12 = 0; //RB12 set as output 
#define ANSELB12 bits.ANSB12 = 0; //RB12 analog functionality disabled 
#define TRISB13 bits.TRISB13 = 0; //RB13 set as output 
#define ANSELB13 bits.ANSB13 = 0; //RB13 analog functionality disabled 
#define TRISA9 bits.TRISA9 = 0;  //RA9 set as output 
#define TRISA10 bits.TRISA10 = 0; //RA10 set as output 
#define TRISG12 bits.TRISG12 = 0; //RG12 set as output 
#define TRISA14 bits.TRISA14 = 0; //RA14 set as output 
#define TRISD6 bits.TRISD6 = 0;  //RD6 set as output 
#define TRISG13 bits.TRISG13 = 0; //RG13 set as output 
#define TRISG15 bits.TRISG15 = 0; //RG15 set as output 
#define TRISD7 bits.TRISD7 = 0;  //RD7 set as output 
#define TRISD13 bits.TRISD13 = 0; //RD13 set as output 
#define TRISG14 bits.TRISG14 = 0; //RG14 set as output

//
#define AN0 LATBbits.LATB12
#define AN1 LATBbits.LATB13
#define AN2 LATAbits.LATA9
#define AN3 LATAbits.LATA10


#define led0 LATAbits.LATA0
#define led1 LATAbits.LATA1
#define led2 LATAbits.LATA2
#define led3 LATAbits.LATA3
#define led4 LATAbits.LATA4
#define led5 LATAbits.LATA5
#define led6 LATAbits.LATA6
#define led7 LATAbits.LATA7

int DELAY = 15;
unsigned char zero = 0b1000000;
unsigned char one = 0b1111001;
unsigned char two = 0b0100100; 
unsigned char three = 0b0110000;
unsigned char four = 0b0011001;
unsigned char five = 0b0010010;
unsigned char six = 0b0000010;
unsigned char seven = 0b1111000;
unsigned char eight = 0b0000000; 
unsigned char nine = 0b0010000; 
unsigned char clear = 0b1111111;

//SSD_Init(); 
unsigned int d0;
unsigned int d1;
unsigned int d2;
unsigned int d3;
unsigned int d4;
int counter1 = 0;
int ms;
int  display[4];
int  decimal[4]={1,1,0,0};
int buttonLock = 0; 
int decimals_time = 1;
int alarm_time = 0;
int time2 = 0; // used
int segments[10];
int alarm_mins = 0;
int alarm_hour = 12;

/* -------------------------Main----------------------------------- */
int main(void) {
    /*---------------Initiate state of clock (00:00) and alarm (12:00) ---------*/
    int hour, mins = 0;
    
    // four digits to display

    while(1){
        
        
        
        
        
        
        
        while(mode == 1){
            led0 = 1;
            led1 = 0;
            int idx;
           
            if (mins > 59)      // Responsible for restricting the minutes
                mins = 0;
            else if (mins < 0)
                mins = 59;
            
            
            
            if (hour > 23)      // responsible for restricting the hours
                hour = 0;
            else if (hour < 0)
                hour = 23;
            display[0] = mins % 10; // code for counter
            display[1] = (mins/10) % 10;
            display[2] = hour % 10;
            display[3] = (hour/10) % 10;
            int i;
            for(i = 0; i < 5; i++){
                toggle_display(display,decimal);
            }
            
            if(btnc && !buttonLock){
                buttonLock = 1;
                mode = 2;
                decimals_time = 1; //decimals
                
                decimal[0] = 1;
                decimal[1] = 1;
                decimal[2] = 0;
                decimal[3] = 0;
            }
            else if(btnl && !buttonLock){
                buttonLock = 1;
                
                for(idx = 0; idx < 4; idx++){
                    decimal[idx] = decimal[idx] ^ 1;
                }
                decimals_time = decimals_time ^1;
            }
            else if(btnu && !buttonLock){
                if (decimals_time == 0){
                    mins ++;
                }
                else 
                    hour++;
            
            buttonLock = 1;
            }  
            else if(btnd && !buttonLock){
                if (decimals_time == 0){
                    mins --;
                }
                else 
                    hour--;
            
            buttonLock = 1;
            }   
            else if(!btnc && !btnd && !btnl && !btnr && !btnu && buttonLock)
                buttonLock = 0;
        
        }
        
        
        
        
        
        
        
        
        
        
        while (mode == 2){
             
            led0 = 0;
            led1 = 1;
            int idx;
            
            if (alarm_mins > 59)      //Restrictions on time
                alarm_mins = 0;
            else if (alarm_mins < 0)
                alarm_mins = 59;
            
            
            if (alarm_hour > 23)       //Restrictions on time
                alarm_hour = 0;
            else if (alarm_hour < 0)
                alarm_hour = 23;
            display[0] = alarm_mins % 10;
            display[1] = (alarm_mins/10) % 10;
            display[2] = alarm_hour % 10;
            display[3] = (alarm_hour/10) % 10;
            int i;
            for(i = 0; i < 5; i ++){
                toggle_display(display,decimal);
            }
            
            if (btnc && !buttonLock){
                buttonLock = 1;
                mode = 3;
                
                for(idx = 0; idx < 4;idx ++){
                    decimal[idx] = 1;
                }
            }
            
            else if(btnr && !buttonLock){
                buttonLock = 1;
                mode = 1;
                decimals_time = 1;
                
                
                decimal[0] = 1;
                decimal[1] = 1;
                decimal[2] = 0;
                decimal[3] = 0;
                
                
            }
            
            else if(btnl && !buttonLock){
                buttonLock = 1;
                
                for (idx = 0; idx < 4; idx ++);{
                    decimal[idx] = decimal [idx] ^ 1;
                                               }
                decimals_time = decimals_time ^1;
            
            }
            
            else if (btnu && !buttonLock){
                if(decimals_time == 0)
                    alarm_mins++;
                else
                    alarm_hour++;
                buttonLock = 1;
            }
            else if (btnd && !buttonLock){
                if(decimals_time == 0)
                    alarm_mins--;
                else
                    alarm_hour--;
                buttonLock = 1;
            }
            
        }
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        while(mode == 3){
            led0 = 0;
            led1 = 0;
            
            
            if (mins > 59)      // Responsible for restricting the minutes
                mins = 0;
            else if (mins < 0)
                mins = 59;
            
            
            
            if (hour > 23)      // responsible for restricting the hours
                hour = 0;
            else if (hour < 0)
                hour = 23;
            
            display[0] = mins % 10;
            display[1] = (mins/10) % 10;
            display[2] = hour % 10;
            display[3] = (hour/10) % 10;
            int i;
            for (i = 0; i < 5; i++){
                toggle_display(display,decimal);
                time2 = time2 + DELAY;
            }
            if(btnc && !buttonLock){
                buttonLock = 1;
                mode = 2;
                decimal[0] = 1;
                decimal[1] = 1;
                decimal[2] = 0;
                decimal[3] = 0;
                int idx;
                for(idx = 0; idx < 4;idx ++){
                    decimal[idx] = 0;
                }
            }
            else if(btnr && !buttonLock){
                buttonLock = 1;
                mode = 1;
                decimals_time = 1;
                
                
                decimal[0] = 1;
                decimal[1] = 1;
                decimal[2] = 0;
                decimal[3] = 0;
            
        }
        
        }
        while (mode == 4){
            int n;
            for (n = 0; n < 4; n++){
                decimal[j] = 1;
            }
            int l;
            for(l=0;l<4;l ++){
                display[l] = 10;
            }
            int k;
            for(k = 0;k < 16; k++ ){
                led0 = 0;
                led1 = 0;
                toggle_display(display,decimal);
                time2 =  time2 + DELAY;    
            }
            
            
            if (mins > 59)      // Responsible for restricting the minutes
                mins = 0;
            else if (mins < 0)
                mins = 59;
            
            
            
            if (hour > 23)      // responsible for restricting the hours
                hour = 0;
            else if (hour < 0)
                hour = 23;
            
            display[0] = mins % 10;
            display[1] = (mins/10) % 10;
            display[2] = hour % 10;
            display[3] = (hour/10) % 10;
            
            if (time2 > 1000){
                time2 = 0;
                mins++;
                alarm_time++;
                
            }
            
            if( alarm_time > 10){
                mode = 3;
                led0 = 0;
                led1 = 0;
            }
            int i;
            for(i = 0;i < 16; i ++ ){
                led0 = 1;
                led1 = 1;
                toggle_display(display,decimal);
                time2 = time2 + DELAY;
            }
            if (mins > 59)      // Responsible for restricting the minutes
                mins = 0;
            else if (mins < 0)
                mins = 59;
            
            
            
            if (hour > 23)      // responsible for restricting the hours
                hour = 0;
            else if (hour < 0)
                hour = 23;
            
            if(btnc){
                toggle_display(display,decimal);
                if(btnr){
                    mode = 3;
                    led0 = 0;
                    led1 = 0;
                    buttonLock = 1;
                }
            }
            if(btnr){
                toggle_display(display,decimal);
                if(btnc){
                    mode = 3;
                    led0 = 0;
                    led1 = 0;
                    buttonLock = 1;
                }
            }
        }
    
    
    
    
    
    
    }
                

                 

                    /*if (counter > 9999) {
                        counter = 0;
                    }
                    d1 = counter % 10;
                    d2 = (counter / 10) % 10;
                    d3 = (counter / 100) % 10;
                    d4 = (counter / 1000) % 10;
                    SSD_WriteDigits(d1, d2, d3, d4, 0, 0, 0, 0);
                    delay_ms(200);
                    counter++;
                }
        */
    
//}

/* ------------------------------------------------------------ 
 **	delay_ms
 **	Parameters:
 **		ms - amount of milliseconds to delay (based on 80 MHz SSCLK)
 **	Return Value:
 **		none
 **	Description:
 **		Create a delay by counting up to counter variable
 ** ------------------------------------------------------------ */
void delay_ms(DELAY) {

    int counter;
    for (counter = 0; counter < DELAY; counter++) {
        for (j = 0; j < 1426; j++) {
        } //software delay 1 millisec
    }
}

/*void showNumber(int display_value){
    int i;
    int number;
    AN0=0;AN1 = AN2= AN3 =1 ; // 1. selectdigit0(anodes)
    displayDigit( number[display_value%10] );// 2. drive all the segments
    for (int i = 0; i < REFRESH_RATE; i++);// refresh
    AN1=0;
    AN0= AN2=AN3=1; // 1. selectdigit0(anodes)
    displayDigit(number[display_value/10] );// 2. drive all the segments
    for (int i = 0; i < REFRESH_RATE; i++);// refresh
    
    
    
} */

/*void displayDigit(unsigned char value){
    SegCA = value & 1;
    SegCB = (value>> 1) & 1;
    SegCC = (value>> 2) & 1;
    SegCD = (value>> 3) & 1;
    SegCE = (value>> 4) & 1;
    SegCF = (value>> 5) & 1;
    SegDF = (value>> 6) & 1;
}*/
void SSD_Timer1Setup()
{
    int PB_FRQ = 1;
    int TMR_TIME = 1;
  PR1 = (int)(((float)(TMR_TIME * PB_FRQ) / 256) + 0.5); //set period register, generates one interrupt every 3 ms
  TMR1 = 0;                           //    initialize count to 0
  T1CONbits.TCKPS = 2;                //    1:64 prescale value
  T1CONbits.TGATE = 0;                //    not gated input (the default)
  T1CONbits.TCS = 0;                  //    PCBLK input (the default)
  T1CONbits.ON = 1;                   //    turn on Timer1
  IPC1bits.T1IP = 7;                  //    priority
  IPC1bits.T1IS = 3;                  //    subpriority
  IFS0bits.T1IF = 0;                  //    clear interrupt flag
  IEC0bits.T1IE = 1;                  //    enable interrupt
  //macro_enable_interrupts();
}
/*void SSD_WriteDigits(unsigned char d1, unsigned char d2, unsigned char d3, unsigned char d4, \
        unsigned char dp1, unsigned char dp2, unsigned char dp3, unsigned char dp4)
{
      T1CONbits.ON = 0;                   // turn off Timer1
    digits[0] = SSD_GetDigitSegments(d1);
    digits[1] = SSD_GetDigitSegments(d2);
    digits[2] = SSD_GetDigitSegments(d3);
    digits[3] = SSD_GetDigitSegments(d4);
    
    if(!dp1)
    {
        digits[0] |= 0x80;
    }
    if(!dp2)
    {
        digits[1] |= 0x80;
    }
    if(!dp3)
    {
        digits[2] |= 0x80;
    }
    if(!dp4)
    {
        digits[3] |= 0x80;
    }    
  T1CONbits.ON = 1;                   //  turn on Timer1
}
/*unsigned char SSD_GetDigitSegments(unsigned char d)
{
    unsigned char bResult = -1;
    if(d < 0)
    {
        bResult = digitSegments[d];
    }
    return bResult;
}*/


}
void toggle_display(int * display, int  * decimal){
    unsigned char segments[]={zero,one,two,three,four,five,six,seven,eight,nine,clear};
    int idx;
    for(idx = 0; idx < 4; idx++){   
        int num_disp = segments[display[idx]];
        AN0 = 0;
        AN1 = 1;
        AN2 = 1; //select digit anodes
        AN3 = 1;
    
        switch(idx){  //activate current digit anodes
            case 0:
                AN0 = 0;    //activate anodes one by one
                break;
            case 1:
                AN1 = 0;
                break;
            case 2:
                AN2 = 0;
                break;
            case 3:
                AN3 = 0;
                break;
             
        }
        SegCA= num_disp;     //display a single digit
        SegCB= num_disp >> 1;
        SegCC= num_disp >> 2;
        SegCD= num_disp >> 3;
        SegCE= num_disp >> 4;
        SegCF= num_disp >> 5;
        SegCG= num_disp >> 6;
        SegDF= decimal[idx];
        
    }
}
