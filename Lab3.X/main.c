/*
 * File:   Lab 2
 * Author: Pablo Herrarte
 * Curso: Electr�nica Digital 2 
 * Fecha: 27/07/2021
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

/**************************PALABRAS DE CONFIGURACI�N**************************/
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
/******************************LIBRER�AS***************************************/
#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
#include "ADC1.h"                //ADC
#include "USART.h"              //CONFIGURACI�N USART
#define _XTAL_FREQ 4000000      //Frecuencia a trabajar
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/****************************VARIABLES***************************************/
uint8_t nowadc = 0; //Variables para controlar adc
uint8_t anlec = 0;
uint8_t v1 = 0; //Valores de los potenci�metros
uint8_t v2 = 0;
uint8_t T = 1;  //Toggle para controlar dos se�ales anal�gicas

/***********************PROTOTIPO FUNCIONES**********************************/
void Setup(void);   //Setup
void ADCL(void);    //Lectura ADC

/*****************************INTERRUPCIONES***********************************/
void __interrupt() ISR(void){
    if(INTCONbits.TMR0IF == 1){ //Interrupci�n del timer 0
        INTCONbits.TMR0IF = 0;
        TMR0 = 236;
        nowadc++; //Se va sumando el valor de la variable de la ADC cada vez que el timer 0 hace un ciclo
    }
    if (PIR1bits.ADIF == 1){ //Si la conversi�n AD fue realizada se regresa la bandera a 0
        PIR1bits.ADIF = 0;
        anlec = ADRESH; //Se�al anal�gica        
    }
}

/******************************CICLO*******************************************/
void main(void) {
    Setup();    //Setup
}

/***************************************FUNCIONES******************************/
void Setup(void){
    //CONFIG I&0
    ANSEL = 0;
    ANSELH = 0;
    ANSEL = 0b00000011; //Puertos anal�gicos y digitales
    TRISA = 0b00000011; //Inputs para las se�ales anal�gicas
    TRISB = 0;  //Outputs
    TRISC = 0b10000000; //Input para RX
    TRISD = 0; //Outputs
    TRISE = 0; //Outputs
    PORTA = 0; //Potenciometros
    PORTB = 0; 
    PORTC = 0; //RX y TX
    PORTD = 0; //Pines de LCD. (D0 a D7)
    PORTE = 0; //Pines de LCD. (RS, En, RW)
    INTCONbits.TMR0IF = 0; //Interrupciones
    INTCONbits.RBIF = 0;
    INTCONbits.GIE = 1; //Habilitar interrupciones
    INTCONbits.PEIE = 1; 
    INTCONbits.T0IE = 1; //Interrupci�n del timer 0
    INTCONbits.RBIE = 0;
    PIR1bits.ADIF = 0; //Funci�n AD lisa para comenzar
    OSCCONbits.IRCF0 = 0; //Configuraci�n del oscilador (4MHz)
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    ADC_init(20, 20);   //Se escoge velocidad y canal para ADC
    ADCON1 = 0;
    TMR0 = 236; //Donde comienza el timer 0
    OPTION_REG = 0b01010111; //Configuraci�n de timer 0 y pull ups
    PIE1bits.ADIE = 1;  //Habilitar ADC
    PIE1bits.RCIE = 1;  //Habilita EUSART Recieve Interrupt
    PIE1bits.TXIE = 1;  //Habilita EUSART Transmit Interrupt
}
void ADCL(void){ //Funci�n de ADC
    if (nowadc > 5){ //Si la variable nowadc es mayor a 5 regresar� a 0 y comienza la converci�n AD
     nowadc = 0;
     ADCON0bits.GO_nDONE = 1;
        if (T==0){ //Se crea un toggle para leer ambos potenci�metros
         ADC_init(1, 20); //Se escoge el canal
         v1 = anlec;  //Se lee se�al anal�gica
         __delay_us(100);
         T = 1;
    }
        else{
         ADC_init(1, 1); //Se escoge el otro canal
         v2 = anlec; //Se lee se�al anal�gica
         __delay_us(100);
         T = 0;
        }    
    }
}
