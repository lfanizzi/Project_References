#include "lcd.h"
#include "led.h"
#include "ssd.h"
#include "utils.h"
#include <plib.h>
#define R4 LATCbits.LATC14   // output latches
#define R3 LATDbits.LATD0
#define R2 LATDbits.LATD1
#define R1 LATCbits.LATC13

#define C4 PORTDbits.RD9  //input ports
#define C3 PORTDbits.RD11
#define C2 PORTDbits.RD10
#define C1 PORTDbits.RD8

#define zero  0b1000000 //numbers defined
#define one  0b1111001
#define two  0b0100100 
#define three  0b0110000
#define four  0b0011001
#define five  0b0010010
#define six  0b0000010
#define seven  0b1111000
#define eight  0b0000000 
#define nine  0b0010000
#define clear  0b1111111 // blank display (all off)
#define A  0b0001000 // A
#define B  0b0000011 // b
#define C  0b1000110 // C
#define D  0b0100001 // d
#define E  0b0000110 // E
#define F  0b0001110  // F
#define H  0b0001001  // H
#define symbol  0b0111111  // -symbol
#pragma config OSCIOFNC =	ON
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config FPBDIV = DIV_8           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
#ifndef _SUPPRESS_PLIB_WARNING          //suppress the plib warning during compiling
#define _SUPPRESS_PLIB_WARNING
#endif
#define btnc PORTFbits.RF0
//display section

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
#define SegCG LATDbits.LATD13
#define SegDP LATGbits.LATG14
int size = 20;
int display[20];

int decimal[4] = {1,1,0,0};
unsigned char d0 = zero;
unsigned char d1 = clear;
unsigned char d2 = clear;
unsigned char d3 = clear;

int segments[19]={zero,one,two,three,four,five,six,seven,eight,nine,clear,A,B,C,D,E,F,H,symbol};
int num_manager[20];
int partial_sum;
int digit;
int current_num;

void update_display(void);
void CNConfig(void);
void clear_SSD(void);
int addition(int operand1,int operand2);
int subtraction (int operand1, int operand2);
unsigned char hex(int num);
int dec(unsigned char num);
void change_display();
int hex_ = 0;

int main(void){
    display[0] = 0;
display[1] = 10;
display[2] = 10;
display[3] = 10;
    CNConfig();
TRISDbits.TRISD0 = 0;
TRISDbits.TRISD1 = 0;
ANSELDbits.ANSD1 = 0;    //AN pin shared with D1
TRISCbits.TRISC14 = 0;
TRISCbits.TRISC13 = 0;
TRISDbits.TRISD8 = 1;
TRISDbits.TRISD9 = 1;
TRISDbits.TRISD10 = 1;
TRISDbits.TRISD11 = 1;
LCD_Init();
LED_Init();
SSD_Init();

//SSD_Timer1Setup();
IEC1bits.CNDIE = 1;

//CN_Config();
CNCONDbits.ON= 1;
CNEND=0xf00;
CNPUD=0xf00;
//IEC1SET = 0x8000;
/*d0 = zero;
d1 = clear;
d2 = clear;
d3 = clear;*/





LCD_WriteStringAtPos("Calculation", 0, 3);
LCD_WriteStringAtPos("Decimal", 1, 3);


while(1){
    change_display();
   toggle_display(display,decimal);
   
    
   
}

}




void __ISR(_CHANGE_NOTICE_VECTOR) CN_Handler(void) {
    
IEC1bits.CNDIE = 0;   //Disable change notice interrupt on port D...
int i;
for(i=0;i<1000;i++){
}

//LCD_WriteStringAtPos("a", 0, 0);
R1 = 0; R2 = R3 = R4 = 1;
if(C1 == 0){ LCD_WriteStringAtPos("A", 0, 0);
    change_display(1);}
else if(C2 == 0){change_display(2);}
else if(C3 == 0){change_display(3);}
else if(C4 == 0){
addition(current_num, partial_sum);}

R2 = 0; R1 = R3 = R4 = 1;
if(C1 == 0){change_display(4);}
else if(C2 == 0){change_display(5);}
else if(C3 == 0){change_display(6);}
else if(C4 == 0){subtraction(current_num, partial_sum);}
R3 = 0; R2 = R1 = R4 = 1;
if(C1 == 0){change_display(7);}
else if(C2 == 0){change_display(8);}
else if(C3 == 0){change_display(9);}
else if(C4 == 0){clear_SSD();
current_num = 0;
partial_sum = 0;}
R4 = 0; R2 = R3 = R1 = 1;
if(C1 == 0){change_display(0);
}
else if(C2 == 0){
if(!hex_){
    hex(current_num);
    LCD_DisplayClear();
    LCD_WriteStringAtPos("Calculation", 0, 3);
    LCD_WriteStringAtPos("Hexadecimal", 1, 3);
hex_ = 1;}
if (hex_){
    dec(current_num);
    LCD_DisplayClear();
    LCD_WriteStringAtPos("Calculation", 0, 3);
    LCD_WriteStringAtPos("Decimal", 1, 3);
    hex_ = 0;
}
}
else if(C3 == 0){change_display(segments[15]);}
else if(C4 == 0){change_display(segments[14]);}
        
R1 = R2 = R3 = R4 = 0;
PORTD;     //readporttoclearmismatchon
IFS1bits.CNDIF = 0;   //Clear interrupt flag status  
IEC1bits.CNDIE = 1;  //Re-enablechange notice interrupt on port D


    }





void delay_ms(int DELAY) {

    int counter;
    for (counter = 0; counter < DELAY; counter++) {
        int j;
        for ( j = 0; j < 1426; j++) {
        } //software delay 1 millisec
    }
}
void change_display(){
    //current_num += new_component;
    int j;
    for(j=0;j<size;j++){
           display[j] = display[j+1];// Problem with assignments, need to look over logic
    }
           //display[0] = new_component;
           display[0] = current_num % 10;
            display[1] = (current_num/10) % 10;
            display[2] = current_num % 10;
            display[3] = (current_num/10) % 10;
            
           d0 = SSD_GetDigitSegments(display[0]);
           d1 = SSD_GetDigitSegments(display[1]);
           d2 = SSD_GetDigitSegments(display[2]);
           d3 = SSD_GetDigitSegments(display[3]);
           
           
                   
           SSD_WriteDigits(d3,d2,d1,d0,0,0,0,0);
}
void update_display(void){
     //d0 = SSD_GetDigitSegments(display[0]);
     //d1 = SSD_GetDigitSegments(display[1]);
     //d2 = SSD_GetDigitSegments(display[2]);
     //d3 = SSD_GetDigitSegments(display[3]);
     //SSD_WriteDigits(one,two,three,four,0,0,0,0);
    SSD_WriteDigits(d3,d2,d1,d0,0,0,0,0);
}


int addition(int operand1,int operand2){
    current_num = partial_sum;
current_num = 0;

int sum = operand1 + operand2;
return sum;
}

int subtraction (int operand1, int operand2){
    int difference = operand1 - operand2;
return difference;
}

//void multiplication (int operand1, int operand2){
    
//}


void clear_SSD(void){
    int idx;
    for(idx = 0; idx<=20; idx ++){
    display[idx]=10;}
}

unsigned char hex(int num){
    // convert num to hex
    
    
}

int dec(unsigned char num){
  // convert num to decimal  
    
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

void toggle_display(int * display, int  * decimal){
    unsigned char segments[]={zero,one,two,three,four,five,six,seven,eight,nine,clear};
    int idx;
    for(idx = 0; idx < 4; idx++){   
        int num_disp = segments[display[idx]];
        AN0 = 1;
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
        SegCA= num_disp &1;     //display a single digit
        SegCB= (num_disp >> 1)&1;
        SegCC= (num_disp >> 2)&1;
        SegCD= (num_disp >> 3)&1;
        SegCE= (num_disp >> 4)&1;
        SegCF= (num_disp >> 5)&1;
        SegCG= (num_disp >> 6) &1;  
        SegDP= decimal[idx] &1;
        delay_ms(1);
    }
}