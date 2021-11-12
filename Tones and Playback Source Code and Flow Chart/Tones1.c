#ifndef _SUPPRESS_PLIB_WARNING
    #define _SUPPRESS_PLIB_WARNING
#endif
#include <plib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xc.h>   //Microchip XC processor header which links to the PIC32MX370512L header
#include "config.h"// Basys MX3 configuration header
#include "led.h"    // Digilent Library for using the on-board LEDs
#include "btn.h"    // Digilent Library for using the on-board BTNs
#include "swt.h"    // Switches
#include "lcd.h"    // LCD
#include "adc.h"
//5499 for array
// 10 to 1023    3.3  / 1023   3.23mv 10 bit
// 0 to 4095 resolution = 3.3/4095 = .806mV  12 bits for each data, short int for each
// 5 seconds - how many samples will you take?? 55,000 is 11k * 5
// 10 bit ADC connected to 16 GPIO pins, AN0 thru AN15 port B, pins that correspond to a
//
#define SYS_FREQ  (80000000L)
#define PB_CLK SYS_FREQ / 8
#define SAMPLE_RATE 11000

int i = 0;
int max_range = 1023;
int speaker_lock = 0;
int buttonlock = 0;
int mode = 0;
unsigned short sample[55000];
float duty_cycle = 0;
unsigned short A4[] = {512,639,758,862,943,998,1022,1014,974,906,812,700,576,447,323,211,117,49,9,1,25,80,161,265,384,512};
unsigned short B5[] = {512,654,785,895,974,1016,1019,982,907,801,673,531,388,255,142,58,10,2,34,104,206,332,473};
unsigned short C5[] = {512,662,799,911,987,1022,1010,955,860,735,589,437,291,165,70,13,1,34,110,221,358,508};
unsigned short D5[] = {512,680,830,944,1010,1020,974,876,738,574,403,245,116,31,0,26,106,231,387};
unsigned short E5[] = {512,700,861,974,1022,998,907,760,578,387,213,81,10,8,78,208,380};
unsigned short F5[] = {512,710,878,988,1023,978,859,686,486,290,128,27,1,56,183,361};
unsigned short G5[] = {512,733,911,1010,1011,913,736,515,293,114,14,12,108,284,505};
unsigned short A5[] = {512,758,943,1022,974,812,576,323,117,9,25,161,384};
int term = 0;
#pragma config OSCIOFNC = ON
#pragma config FNOSC = PRIPLL
#pragma config POSCMOD = XT 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLMUL = MUL_20 
#pragma config FPLLODIV = DIV_1 
#pragma config FPBDIV = DIV_8
#pragma config FSOSCEN = OFF
int time_manager = 0;
char time[] = "";
int counter = 0;
unsigned short* sound;

void configure_Project();
void play_sound();
void play_sound_reboot();
void recording_studio();
void playback();

void main (void){
    configure_Project();
    //LCD_WriteStringAtPos("1: Tone Play", 0, 0);
   while(1) {
       //LCD_WriteStringAtPos("1: Tone Play", 0, 0);
       //LCD_DisplayClear();
       switch(mode){
           case 0:
               //LCD_DisplayClear();
               LCD_WriteStringAtPos("1: Tone Play", 0, 0);
               if(BTN_GetValue('C')&& !buttonlock) {
        mode = 1;
        buttonlock = 1;
        delay_ms(200);
        LCD_DisplayClear();
    }
               buttonlock = 0;
               break;
   case 1: 
       
       //LCD_DisplayClear();
   LCD_WriteStringAtPos("2: Recording", 0, 0);
   LCD_WriteStringAtPos("Seconds:" , 1, 0);
   LCD_WriteStringAtPos(time, 1, 9);
   
   
   if(BTN_GetValue('C')&& !buttonlock) {
        mode = 2;
        buttonlock = 1;
        delay_ms(200);
        LCD_DisplayClear();
    }
   buttonlock = 0;
   break;
   case 2:
       
       
   LCD_WriteStringAtPos("3: Playback", 0, 0);
   LCD_WriteStringAtPos("Time left:", 1, 0);
   
   if(BTN_GetValue('C')&& !buttonlock) {
        mode = 0;
        buttonlock = 1;
        delay_ms(200);
        LCD_DisplayClear();
    }
   break;
   buttonlock = 0;
       } 
   
   
   
    
   // logic for playing tone
  
/*
if(SWT_GetValue(7)&&!speaker_lock){
    OC2CONbits.ON = 1;
    sound  = A4;
    //play_sound();
    LEDS_Led7SetValue(1);
    speaker_lock = 1;
}
       
      
else{
    LED_SetGroupValue(0);
    OC2CONbits.ON = 0;
    speaker_lock = 0;
    sound = 0;
    //play_sound();
}*/
   }
   
}





void configure_Project(){
    SWT_Init();
    LED_Init();
    BTN_Init();
    LCD_Init();
    ADC_Init();
    DDPCONbits.JTAGEN = 0;
//Timer
 
TMR2 = 0;
//T2CON  = 0x8030;  // reset timer 2 configuration
//TMR2 = 0x8;     //  reset time counter register
PR2 = (int)((((float)PB_CLK)/11000) + 0.5);     // set timer2 period to 11kHZ
T2CONbits.TCKPS = 0;//1:1 prescalevalue 2
T2CONbits.TGATE = 0;//not gated input (default)
T2CONbits.TCS = 0;//PCBLK input (default)
T2CONbits.ON = 1; //turn on Timer2
IPC2bits.T2IP= 7;//priority
IPC2bits.T2IS = 3;//subpriority 3 --6
IFS0bits.T2IF = 0;//clear interrupt flag
IEC0bits.T2IE = 1;//enable interrupt
//INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
//INTEnableInterrupts();
macro_enable_interrupts();
//T2CONSET = 0x800; // start timer2 and set bit 15 as the control register


// microphone sampling
/*AD1CON1 = 0x00E0; // automatic conversion after sampling
AD1CHS = 0x00040000; // Connect RB4/AN4 as CH0 input
AD1CSSL = 0; // no scanning required
AD1CON2 = 0; // use MUXA, AVss/AVddused as Vref+/-
AD1CON3 = 0x1F3F; // Tad = 128 x Tpb, Sample time = 31 Tad
AD1CON1bits.ON = 1; // turn on the ADC*/

//OC2
OC2CONbits.ON = 0;
OC2CONbits.OCM =  6;
OC2CONbits.OCTSEL =  0;
OC2CONbits.ON = 1;
// Connecting OC2 to speaker
TRISBbits.TRISB14 = 0; 
ANSELBbits.ANSB14 = 0;
RPB14R = 0xB; 

// other values 

//OC2R = 0;

//PR2 = (int)((float)((float)PB_CLK/SAMPLE_RATE)-1);

//FPBDIV = DIV_4;


}





void __ISR(_TIMER_2_VECTOR, IPL7SOFT) Timer2Handler(void) {
//IFS0bits.T2IF = 1;
    switch(mode){
            case 0:
    play_sound();
    
break;

    case 1:
        recording_studio();
        break;
        
    case 2:
        playback();
        //LEDS_Led3SetValue(1);
        break;
        
    
    }
 
 

IFS0bits.T2IF = 0;


}



//mode 2
// if statement())
//drops out, save sample
//if pressed again, make sure sample is set to 0 to reset







void play_sound(){
    T2CONbits.ON = 1;
    if (SWT_GetValue(0)&& !speaker_lock){
        OC2CONbits.ON = 1;
        sound = A4;
        LEDS_Led0SetValue(1);
        speaker_lock = 1;
    }
    if(SWT_GetValue(1)&&!speaker_lock){
        OC2CONbits.ON = 1;
        sound = G5;
    
        LEDS_Led1SetValue(1);
        speaker_lock = 1;
    }
    if(SWT_GetValue(2)&&!speaker_lock){
    OC2CONbits.ON = 1;
    
    sound = F5;
    LEDS_Led2SetValue(1);
    speaker_lock = 1;
}
    else if (SWT_GetValue(3)&&!speaker_lock){
    OC2CONbits.ON = 1;
    
    sound = E5;
    LEDS_Led3SetValue(1);
    speaker_lock = 1;
}
    else if(SWT_GetValue(4)&&!speaker_lock){
    OC2CONbits.ON = 1;
    

    sound = D5;
    LEDS_Led4SetValue(1);
    speaker_lock = 1;
}
    else if(SWT_GetValue(5)&&!speaker_lock){
    OC2CONbits.ON = 1;
    

    sound = C5;
    LEDS_Led5SetValue(1);
    speaker_lock = 1;
}
    else if(SWT_GetValue(6)&&!speaker_lock){
    OC2CONbits.ON = 1;
    
   sound = B5;
    LEDS_Led6SetValue(1);
    speaker_lock = 1;
}
else if(SWT_GetValue(7)&&!speaker_lock){
    OC2CONbits.ON = 1;
    
    sound = A5;
    LEDS_Led7SetValue(1);
    speaker_lock = 1;
}
       
      
    else{ //if(!SWT_GetValue(7)&&!SWT_GetValue(6)&&!SWT_GetValue(5)&&!SWT_GetValue(4)&&!SWT_GetValue(3)&&!SWT_GetValue(2)&&!SWT_GetValue(1)&&!SWT_GetValue(0)){
    i = 0;
    LED_SetGroupValue(0);
    OC2CONbits.ON = 0;
    speaker_lock = 0;
    OC2RS = 0;
    
}
    
  
    
    
    
    int i;
    
    for(i = 0; i <= sizeof(sound)/sizeof(sound[0]); i++){
    OC2RS = (int) ((float) (sound[i] *PR2 +1)/ (float)max_range) - 1;
}
}

void recording_studio(){
   // set correct bits on and to start microphone
    // mode 2 
       // start and stop timerr, store input fro m microphone into the array
       // increment through array while holding button R using pointer
       // 
    //T2CONbits.ON = 1;
   //T2CONbits.ON = 1;
    //OC2CONbits.ON = 0;
    //if(i==0 && !BTN_GetValue('R')){
        //T2CONbits.ON = 1;
    //}
    
    if(BTN_GetValue('R')&& !buttonlock){
        T2CONbits.ON = 1;
        sample[i]= ((int)ADC_AnalogRead(4));
        i++;
        LEDS_Led2SetValue(1);
        if(counter>=142600){
        time_manager += .1;
        counter = 0;
        sprintf(time,"%d.%d", (int) time_manager, (int)time*10%10);
        }
        
        if(i >= 54999 || !BTN_GetValue('R')){
            i = 0;
            counter = 0;
            LEDS_Led2SetValue(0);
            T2CONbits.ON = 0;
            
            
            
            
        }
        
            
    }
    
}
void playback(){
    T2CONbits.ON = 1;
    LEDS_Led3SetValue(1);
    OC2CONbits.ON = 1;
    //T2CONbits.ON = 1;
   
    OC2RS = sample[i++];
    counter++;
    if(counter>=142600){
        time_manager -= .1;
        counter = 0;
    }
    if(i>=55499){
      OC2CONbits.ON = 0;
      i = 0;
      LEDS_Led3SetValue(0);
      
      
    }
    
}


void delay_ms(int ms){
    

int counter;
for (counter=0; counter< ms; counter++)
{
    int i;
    for(i=0;i<1426;i++){} ;  //software delay 1 millisec}}

}
}

short readADC( ){
    
AD1CON1bits.SAMP = 1; // 1. start sampling
while(AD1CON1bits.SAMP);  // 2. wait until acquisition is done
while (!AD1CON1bits.DONE); // 3. wait conversion completereturn 
ADC1BUF0; // 4. read conversion result
}





















































void play_sound_reboot(){
    
    
       
    if (SWT_GetValue(0)&& !speaker_lock){
        OC2CONbits.ON = 1;
        //for(i=0; i<=sizeof(A5[i])/sizeof(A5[0]);i++){
        //delay_ms(100);
        //duty_cycle = A5[(++i)%25];
        //pr2 +1 is duty cycle (A5[i]) *( float)(PR2 +1)
         //OC2RS  = (int) ((float) (PR2+1)*((duty_cycle)/ (float)max_range))- 1;
        OC2RS = (int)(PR2 +1) + (A5[i++ %25])/ max_range;
        //OC2RS = A5[i++%25];
         //OC2RS  = (int) ((float) ((A5[i]) *(PR2 +1))/ (float)max_range) - 1; 
        //OC2RS = A5;
        //i++;
        if(i >= (25)){
            i = 0;
        }
        LEDS_Led0SetValue(1);
        speaker_lock = 1;
    }
    else if(SWT_GetValue(1)&&!speaker_lock){
        OC2CONbits.ON = 1;
        
        OC2RS  = (int) ((float) ((B5[i]) *(PR2/2 +1))  - 1); 
        i++;
        //OC2RS = (int)(PR2 +1) + (B5[i++ %25])/ max_range;
        if(i >= sizeof(B5[i])/sizeof(B5[0] )){
            i = 0;
        }
    
        LEDS_Led1SetValue(1);
        speaker_lock = 1;
    }
else if(SWT_GetValue(2)&&!speaker_lock){
    OC2CONbits.ON = 1;
   
    OC2RS  = (int) ((float) (C5[i] *(PR2 +1))/ (float)max_range) - 1;
    i++;
    if(i >= sizeof(C5[i])/sizeof(C5[0] )){
        i = 0;
    }
    LEDS_Led2SetValue(1);
    speaker_lock = 1;
}
else if(SWT_GetValue(3)&&!speaker_lock){
    OC2CONbits.ON = 1;
    
    OC2RS  = (int) ((float) (D5[i] *(PR2 +1))/ (float)max_range) - 1;
    i++;
    if(i >= sizeof(D5[i])/sizeof(D5[0] )){
        i = 0;
    }
    LEDS_Led3SetValue(1);
    speaker_lock = 1;
}
else if(SWT_GetValue(4)&&!speaker_lock){
    OC2CONbits.ON = 1;
    

    OC2RS  = (int) ((float) (E5[i] *(PR2 +1))/ (float)max_range) - 1;
    i++;
    if(i >= sizeof(E5[i])/sizeof(E5[0] )){
        i = 0;
    }
    LEDS_Led4SetValue(1);
    speaker_lock = 1;
}
else if(SWT_GetValue(5)&&!speaker_lock){
    OC2CONbits.ON = 1;
    

    OC2RS  = (int) ((float) (F5[i] *(PR2 +1))/ (float)max_range) - 1;
    i++;
    if(i >= sizeof(F5[i])/sizeof(F5[0] )){
        i = 0;
    }
    LEDS_Led5SetValue(1);
    speaker_lock = 1;
}
else if(SWT_GetValue(6)&&!speaker_lock){
    OC2CONbits.ON = 1;
    
   OC2RS  = (int) ((float) (G5[i] *(PR2 +1))/ (float)max_range) - 1;
    i++;
    if(i >= sizeof(G5[i])/sizeof(G5[0] )){
        i = 0;
    }
    LEDS_Led6SetValue(1);
    speaker_lock = 1;
}
else if(SWT_GetValue(7)&&!speaker_lock){
    OC2CONbits.ON = 1;
    
    OC2RS  = (int) ((float) (A4[i] *(PR2 +1))/ (float)max_range) - 1;
    i++;
    if(i >= sizeof(A4[i])/sizeof(A4[0] )){
        i = 0;
    }
    LEDS_Led7SetValue(1);
    speaker_lock = 1;
}
       
      
else{
    i = 0;
    LED_SetGroupValue(0);
    OC2CONbits.ON = 0;
    speaker_lock = 0;
    OC2RS = 0;
    
}
    
   //if(BTN_GetValue("C")) {
      // mode = 1;
   //}
}
