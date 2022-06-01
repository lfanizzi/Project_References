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
#include "srv.h"


#pragma config OSCIOFNC = ON
#pragma config FNOSC = PRIPLL
#pragma config POSCMOD = XT 
#pragma config FPLLIDIV = DIV_2 
#pragma config FPLLMUL = MUL_20 
#pragma config FPLLODIV = DIV_1 
#pragma config FPBDIV = DIV_2
#pragma config FSOSCEN = OFF
//DEFINE and CONFIG PMOD
//#define R4 LATCbits.LATC14   // output latches
//#define R3 LATDbits.LATD0
//#define R2 LATDbits.LATD1
//#define R1 LATCbits.LATC13

#define R PORTDbits.RD9  //input ports
#define RM PORTDbits.RD11
#define LM PORTDbits.RD10
#define L PORTDbits.RD8

//#define R4 LATCbits.LATC14   // output latches
//#define R3 LATDbits.LATD0
//#define R2 LATDbits.LATD1
//#define R1 LATCbits.LATC13

//#define SYS_FREQ  (80000000L)
#define PB_CLK 40000000
//#define SAMPLE_RATE 110000
#define clear  0b1111111 // blank display (all off)
unsigned short sample[11000];
unsigned short time = 0;
int timer_flag = 0;
int count = 0;
int STP_L, STP_R = 0;
int blink_L, blink_R, counter_L,counter_R;
int start, clapping, clap_count = 0;
unsigned char d1,d2,d3,d4 = clear; 
void configure_Project();
void check_direction();
void time_manager();
void motion_maker();
void Forward();
void Right();
void Left();
void Sharp_Left();
void Sharp_Right();
void Stop();
void Reverse();
void Off_Track();
void signal_start();
void right_blinker();
void left_blinker();
void CNConfig();
int stop_count;
int main (void){
   configure_Project();
   while(1){
     if(SWT_GetValue(4)){
      check_direction();
       }  
  }
   
    
}


void __ISR(_CHANGE_NOTICE_VECTOR) CN_Handler(void) {
    IEC1bits.CNDIE = 0;   //Disable change notice interrupt on port D...
    
    //check_direction();
    
    
    
    
    
    
    
    IFS1bits.CNDIF = 0;   //Clear interrupt flag status  
    IEC1bits.CNDIE = 1;  //Re-enablechange notice interrupt on port D
}


void __ISR(_TIMER_3_VECTOR, IPL7SOFT) Timer3Handler(void) {
    if(timer_flag && count >= 4){
        
        time++;
        count = 0;
    }
    count++;
    time_manager();
    if(!start){
    signal_start();
    }
    //check_direction();
//    if(SWT_GetValue(4)){
//      check_direction();
//       }
    if (!SWT_GetValue(4)){
         
        if(start){
    motion_maker();
        }
    }
   
    stop_count++;
    counter_L++;
    if(counter_L> 1500){
        blink_L = !blink_L;
        counter_L = 0;
    }
    counter_R++;
    if(counter_R > 1500){
        blink_R = !blink_R;
        counter_R = 0;
    
}
//    
    IFS0bits.T3IF = 0;


}

void configure_Project(){
    SWT_Init();
    LED_Init();
    BTN_Init();
    LCD_Init();
    SSD_Init();
    ADC_Init();
    SRV_Init();
    CNConfig();
    DDPCONbits.JTAGEN = 0;
    LED_SetGroupValue(0);
 
    LCD_WriteStringAtPos("The Big Kahuna", 0, 1);
    //LCD_WriteStringAtPos("Travi Boy Fetty", 0, 1);
    // NAME
    
    
    //TMR2 = 0;

    PR3 = PB_CLK/(16 * 50) -1;     // set timer2 period to 11kHZ
    T3CONbits.TCKPS = 4;//1:16 prescale value 16
    T3CONbits.TGATE = 0;//not gated input (default)
    T3CONbits.TCS = 0;//PCBLK input (default)
    T3CONbits.ON = 1; //turn on Timer2
    IPC3bits.T3IP= 7;//priority
    IPC3bits.T3IS = 3;//subpriority 3 --6
    IFS0bits.T3IF = 0;//clear interrupt flag
    IEC0bits.T3IE = 1;//enable interrupt

    macro_enable_interrupts();
//PMOD
TRISDbits.TRISD9 = 1;
TRISDbits.TRISD11 = 1;
TRISDbits.TRISD10 = 1;
TRISDbits.TRISD8 = 1;




//OC2
//OC2CONbits.ON = 0;
//OC2CONbits.OCM =  6;
//OC2CONbits.OCTSEL = 0;
//OC2CONbits.ON = 1;

// Connecting OC2 to speaker
//TRISBbits.TRISB14 = 0; 
//ANSELBbits.ANSB14 = 0;
//RPB14R = 0xB; 



//LCD_DisplayClear();
}


void motion_maker(){ // manages all motion decisions in path finder mode
  

     if(!L && !LM && !RM && !R){ //0
         if(stop_count > 1500){
         Stop();
         }
         //start = 0;
    }
    else if(!L && !LM && !RM && R){ // 1
        Right();
        //Left();
    }
       else if (!L && !LM && RM && !R){ //ERROR //2
        Right();
        //Left();
    }  
    else if(!L && !LM && RM && R){ //3
        Right();
        //Sharp_Left();
    }
     else if(!L && LM && !RM && !R){ //4
         Left();
         //Right();
    }
    else if(!L && LM && !RM && R){//ERROR //5
        //Reverse();
        Left();
        //Right();
    }
     else if(!L && LM && RM && !R){ //ERROR // 6
        Reverse();
    }
     else if(!L && LM && RM && R){ //7
        Sharp_Right();
         //Sharp_Left();
    }
     else if(L && !LM && !RM && !R){ 
         Right();
         //Left();
    }
      else if(L && !LM && !RM && R){ //Straight //9
         Forward();
        
      }
     else if(L && !LM && RM && !R){ //ERROR
        //Reverse();
         Left();
         //Right();
    }
     else if(L && !LM && RM && R){ //ERROR
         Forward();
    }
     else if(L && LM && !RM && !R){ //12
         Right();
         //Left();
    }
     else if(L && LM && !RM && R){ //ERROR 13
        //Reverse();
        Right();
        //Left();
    }
     else if(L && LM && RM && !R){ //14
         //Sharp_Right();
         Sharp_Left();
    }
    else if(L && LM && RM && R){ //15
        Off_Track();
    }
    
     if(STP_L && STP_R && !SWT_GetValue(4)){
        time = 0;
    }   
}



//DEMO
void check_direction(){
    //RIGHT MOTOR DIRECTION
    if(SWT_GetValue(0)&& !SWT_GetValue(1)){
        
        //FWD CW RIGHT MOTOR
        LEDS_Led0SetValue(0);
        LEDS_Led1SetValue(0);
        LEDS_Led2SetValue(1);
        LEDS_Led3SetValue(1);
        LCD_WriteStringAtPos("FWD", 1, 13);
        SRV_SetPulseMicroseconds0(1000);
        STP_R = 0;
       if(!timer_flag){
        timer_flag = 1;
        }
    }
    else if(SWT_GetValue(1)&& !SWT_GetValue(0)){
        //REV CCW RIGHT MOTOR
        LEDS_Led0SetValue(1);
        LEDS_Led1SetValue(1);
        LEDS_Led2SetValue(0);
        LEDS_Led3SetValue(0);
        LCD_WriteStringAtPos("REV", 1, 13);
        SRV_SetPulseMicroseconds0(2000);
        STP_R = 0;
        if(!timer_flag){
        timer_flag = 1;
        }
       
    }
    else if(SWT_GetValue(0)&& SWT_GetValue(1) || !SWT_GetValue(0)&& !SWT_GetValue(1) ){
        //STP RIGHT MOTOR
        LEDS_Led0SetValue(0);
        LEDS_Led1SetValue(0);
        LEDS_Led2SetValue(0);
        LEDS_Led3SetValue(0);
        LCD_WriteStringAtPos("STP", 1, 13);
        SRV_SetPulseMicroseconds0(1500);
       timer_flag = 0;
       STP_R = 1;
    }
    
    //LEFT MOTOR DIRECTION
    
    if(SWT_GetValue(6)&& !SWT_GetValue(7)){
        //set OC4RS to correct signal to
        //FWD CCW LEFT MOTOR
        LEDS_Led4SetValue(0);
        LEDS_Led5SetValue(0);
        LEDS_Led6SetValue(1);
        LEDS_Led7SetValue(1);
        LCD_WriteStringAtPos("FWD", 1, 0);
        SRV_SetPulseMicroseconds1(2000);
        if(!timer_flag){
        timer_flag = 1;
        }
        STP_L = 0;
    }
    else if(SWT_GetValue(7)&& !SWT_GetValue(6)){
        //REV CW LEFT MOTOR
        LEDS_Led4SetValue(1);
        LEDS_Led5SetValue(1);
        LEDS_Led6SetValue(0);
        LEDS_Led7SetValue(0);
        LCD_WriteStringAtPos("REV", 1, 0);
        SRV_SetPulseMicroseconds1(1000);
        if(!timer_flag){
        timer_flag = 1;
        }
        STP_L = 0;
    }
    else if(SWT_GetValue(6)&& SWT_GetValue(7) || !SWT_GetValue(6)&& !SWT_GetValue(7) ){
        //STP LEFT MOTOR
        LEDS_Led4SetValue(0);
        LEDS_Led5SetValue(0);
        LEDS_Led6SetValue(0);
        LEDS_Led7SetValue(0);
        LCD_WriteStringAtPos("STP", 1, 0);
        SRV_SetPulseMicroseconds1(1500);
        timer_flag = 0;
        STP_L = 1;
    }
    if(STP_L && STP_R){
        time = 0;
    }
    
    
}
void signal_start(){ // detect the clapping or use the btn c
if(ADC_AnalogRead(4)>=800){
    clap_count ++;
}
if(clap_count>=2){
    clapping = 1;
}

if(BTN_GetValue('C') || clapping ){
 start = 1;
 clapping = 0;
   }
    
    
    //    int i = 0;
//    sample[i]= ADC_AnalogRead(4);
//        i++;
//        if(i>10999){
//            i = 0;
//             LEDS_Led4SetValue(1);
//            clap_count = 0;
//        }
//        
//        if(sample[i] >= 100){
//            clap_count = clap_count + 1; // ADD DELAY IF NECESSARY
//        }
//        if (clap_count == 2){
//            clapping = 1;
//        }
//    if(BTN_GetValue('C') || clapping ){
//        start = 1;
//         //LEDS_Led4SetValue(3);
//        clapping = 0;
//    }
}
void time_manager(){
    d1  = time % 10;
    d2 = (time / 10) % 10;
    d3 = (time / 100) %10;
    d4 = (time / 1000) % 10;
    if(d1>0 && d2 == 0 && d3 <= 0){
        d2 = clear;
    }
    if(d2>0 && d3 == 0 && d4 <=0){
        d3 = clear;
    }
    if(d3>0 && d4 == 0){
        d4 = clear;
    }
    if(d1 == 0 && d2 ==0 && d3 == 0 && d4 == 0 ){
        d3 = clear;
        d4 = clear;
    }
    SSD_WriteDigits(d1,d2 , d3, d4, \
            0, 1, 0, 0);
}

void Forward(){
    //FWD CW RIGHT MOTOR
        LEDS_Led0SetValue(0);
        LEDS_Led1SetValue(0);
        LEDS_Led2SetValue(1);
        LEDS_Led3SetValue(1);
        LCD_WriteStringAtPos("FWD", 1, 13);
        SRV_SetPulseMicroseconds0(1000);
        STP_R = 0;
       if(!timer_flag){
        timer_flag = 1;
        }
        //FWD CCW LEFT MOTOR
        LEDS_Led4SetValue(0);
        LEDS_Led5SetValue(0);
        LEDS_Led6SetValue(1);
        LEDS_Led7SetValue(1);
        LCD_WriteStringAtPos("FWD", 1, 0);
        SRV_SetPulseMicroseconds1(2000);
        if(!timer_flag){
        timer_flag = 1;
        }
        STP_L = 0;
}
void Right(){
    //FWD CCW LEFT MOTOR
        LEDS_Led4SetValue(0);
        LEDS_Led5SetValue(0);
        LEDS_Led6SetValue(1);
        LEDS_Led7SetValue(1);
        LCD_WriteStringAtPos("FWD", 1, 0);
        SRV_SetPulseMicroseconds1(2000);
        if(!timer_flag){
        timer_flag = 1;
        }
        STP_L = 0;
    

    //STP RIGHT MOTOR
        LEDS_Led0SetValue(0);
        LEDS_Led1SetValue(0);
        LEDS_Led2SetValue(0);
        LEDS_Led3SetValue(0);
        LCD_WriteStringAtPos("STP", 1, 13);
        SRV_SetPulseMicroseconds0(1500);
       
       STP_R = 1;
       right_blinker();
    }

void Sharp_Right(){
    //FWD CCW LEFT MOTOR
        LEDS_Led4SetValue(0);
        LEDS_Led5SetValue(0);
        LEDS_Led6SetValue(1);
        LEDS_Led7SetValue(1);
        LCD_WriteStringAtPos("FWD", 1, 0);
        SRV_SetPulseMicroseconds1(2000);
        
        STP_L = 0;
        
        //REV CCW RIGHT MOTOR
        LEDS_Led0SetValue(1);
        LEDS_Led1SetValue(1);
        LEDS_Led2SetValue(0);
        LEDS_Led3SetValue(0);
        LCD_WriteStringAtPos("REV", 1, 13);
        SRV_SetPulseMicroseconds0(2000);
        STP_R = 0;
        if(!timer_flag){
        timer_flag = 1;
        }
        right_blinker();
}
void Left(){
   //FWD CW RIGHT MOTOR
        LEDS_Led0SetValue(0);
        LEDS_Led1SetValue(0);
        LEDS_Led2SetValue(1);
        LEDS_Led3SetValue(1);
        LCD_WriteStringAtPos("FWD", 1, 13);
        SRV_SetPulseMicroseconds0(1000);
        STP_R = 0;
       if(!timer_flag){
        timer_flag = 1;
        } 
        
        //STP LEFT MOTOR
        LEDS_Led4SetValue(0);
        LEDS_Led5SetValue(0);
        LEDS_Led6SetValue(0);
        LEDS_Led7SetValue(0);
        LCD_WriteStringAtPos("STP", 1, 0);
        SRV_SetPulseMicroseconds1(1500);
        
        STP_L = 1; 
        
        left_blinker();
}
void Sharp_Left(){
    //FWD CW RIGHT MOTOR
        LEDS_Led0SetValue(0);
        LEDS_Led1SetValue(0);
        LEDS_Led2SetValue(1);
        LEDS_Led3SetValue(1);
        LCD_WriteStringAtPos("FWD", 1, 13);
        SRV_SetPulseMicroseconds0(1000);
        STP_R = 0;
       
        //REV CW LEFT MOTOR
        LEDS_Led4SetValue(1);
        LEDS_Led5SetValue(1);
        LEDS_Led6SetValue(0);
        LEDS_Led7SetValue(0);
        LCD_WriteStringAtPos("REV", 1, 0);
        SRV_SetPulseMicroseconds1(1000);
        if(!timer_flag){
        timer_flag = 1;
        }
        STP_L = 0;
       left_blinker();
        
}
void error(){ // going to hard code this into the direction code
    
}
void Stop(){
   //STP LEFT MOTOR
        LEDS_Led4SetValue(0);
        LEDS_Led5SetValue(0);
        LEDS_Led6SetValue(0);
        LEDS_Led7SetValue(0);
        LCD_WriteStringAtPos("STP", 1, 0);
        SRV_SetPulseMicroseconds1(1500);
        
        STP_L = 1; 
        
        //STP RIGHT MOTOR
        LEDS_Led0SetValue(0);
        LEDS_Led1SetValue(0);
        LEDS_Led2SetValue(0);
        LEDS_Led3SetValue(0);
        LCD_WriteStringAtPos("STP", 1, 13);
        SRV_SetPulseMicroseconds0(1500);
       timer_flag = 0;
       STP_R = 1;
}
void Reverse(){
  //REV CCW RIGHT MOTOR
        LEDS_Led0SetValue(1);
        LEDS_Led1SetValue(1);
        LEDS_Led2SetValue(0);
        LEDS_Led3SetValue(0);
        LCD_WriteStringAtPos("REV", 1, 13);
        SRV_SetPulseMicroseconds0(2000);
        STP_R = 0;
        
        //REV CW LEFT MOTOR
        LEDS_Led4SetValue(1);
        LEDS_Led5SetValue(1);
        LEDS_Led6SetValue(0);
        LEDS_Led7SetValue(0);
        LCD_WriteStringAtPos("REV", 1, 0);
        SRV_SetPulseMicroseconds1(1000);
        if(!timer_flag){
        timer_flag = 1;
        }
        STP_L = 0;  
}
void Off_Track(){// backward (for now)
    Reverse();
}

void left_blinker(){
    if(blink_L){
        LEDS_Led4SetValue(1);
        LEDS_Led5SetValue(1);
        LEDS_Led6SetValue(1);
        LEDS_Led7SetValue(1);
    }
    if(!blink_L){
        LEDS_Led4SetValue(0);
        LEDS_Led5SetValue(0);
        LEDS_Led6SetValue(0);
        LEDS_Led7SetValue(0);
    }
//    counter_L++;
//    if(counter_L> 1500){
//        blink_L = !blink_L;
//        counter_L = 0;
//    }
        
    
}
void right_blinker(){
    if(blink_R){
        LEDS_Led0SetValue(1);
        LEDS_Led1SetValue(1);
        LEDS_Led2SetValue(1);
        LEDS_Led3SetValue(1);
    }
    if(!blink_R){
        LEDS_Led0SetValue(0);
        LEDS_Led1SetValue(0);
        LEDS_Led2SetValue(0);
        LEDS_Led3SetValue(0);
    }
//    counter_R++;
//    if(counter_R > 1500){
//        blink_R = !blink_R;
//        counter_R = 0;
//    
//}
}
void CNConfig() {/* Makesurevectorinterruptsisdisabledpriortoconfiguration*/
    
   
CNCONDbits.ON= 1;   //allport DpinstotriggerCNinterrupts
CNEND = 0xF00;      //configurePORTD pins 8-11asCNpins
CNPUD = 0xF00;      //enable pullups on PORTD pins 8-11
PORTD;//readporttoclearmismatchonCNpins
IPC8bits.CNIP = 6;  // set CN priority to  6 
IPC8bits.CNIS = 3;  // set CN sub-priority to 3
IFS1bits.CNDIF = 0;   //Clear interrupt flag status nit
IEC1bits.CNDIE = 1;   //Enable CNinterrupt on port D
INTEnableSystemMultiVectoredInt();






}