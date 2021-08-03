/*
 * File:   Lab 3
 * Author: Pablo Herrarte
 * Curso: Electrónica Digital 2 
 * Fecha: 3/08/2021
 * MASTER
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

/**************************PALABRAS DE CONFIGURACIÓN**************************/
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
/******************************LIBRERÍAS***************************************/
#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
#include "SPI_1.h"
#include "USART_2.h"
#include "ASCII_1.h"
#define _XTAL_FREQ 8000000      //Frecuencia a trabajar

/****************************VARIABLES***************************************/

uint8_t toggleTX = 0;   //contador para mandar datos a la terminal virtual
uint8_t v1 = 0;   
uint8_t v2 = 0;    
uint8_t signo = 0;      //Control del signo para transmitir datos desde la terminal virtual
uint8_t sum = 0;        //Suma del contador
uint8_t res = 0;        //Resta del contador
uint8_t CONT = 0;       //Contador
char Cen1 = 0;  //Dígito de potenciómetros y contador
char Cen2 = 0;
char Cen3 = 0;
char Dec1 = 0;
char Dec2 = 0;
char Dec3 = 0;
char Un1 = 0;   
char Un2 = 0;
char Un3 = 0;
char AC1 = 0;
char AC2 = 0;
char AC3 = 0;
char AD1 = 0;
char AD2 = 0;
char AD3 = 0;
char AU1 = 0;
char AU2 = 0;
char AU3 = 0;


/*************************PROTOTIPO FUNCIONES**********************************/
void setup(void);
void LECT1(void);   //Separación de dígitos y lectura del potenciómetro 1
void LECT2(void);   //Separación de dígitos y lectura del potenciómetro 1
void envio(void);   //Datos que recibirá la terminal de lectura
void CONTADOR(void);    //Antirrebote del contador

/*****************************INTERRUPCIONES***********************************/
void __interrupt() ISR(void){
    if(INTCONbits.TMR0IF == 1){ //Interrupción del timer 0
        INTCONbits.TMR0IF = 0;
        TMR0 = 216;
    }
    if (PIR1bits.TXIF == 1){    //El transmit buffer del EUSART está vacío
        envio();    //Se mandará caracter por caracter con esta función
        PIE1bits.TXIE = 0;
    }
    if (PIR1bits.RCIF == 1){    //El recieve buffer del EUSART está lleno4
        signo = RCREG; //El signo será el dato recivido
    }
}    
/******************************CICLO*******************************************/
void main(void) {
    setup();
                     //Configuración de EUSART
    while(1){
       PORTCbits.RC2 = 0;       //Slave Select
       __delay_ms(1);
       spiWrite(0);         //
       v1 = spiRead();       //Estos son los datos analógicos
       __delay_ms(1);
       PORTCbits.RC2 = 1;      //Slave Deselect 
       PORTCbits.RC2 = 0;       //Slave Select
       __delay_ms(1);
       spiWrite(0);         //
       v2 = spiRead();       //Estos son los datos analógicos
       __delay_ms(1);
       PORTCbits.RC2 = 1;      //Slave Deselect 
       if(signo != 13 && signo != 43 && signo != 45){  //Si el caracter ingresado no es + - o enter, no se sumará ni restará
            sum = 0;
            res = 0;
        }
        CONTADOR(); //Contar leer y hacer conversión de datos
        LECT1();  
        LECT2();
    }
    return;
}
/***************************************FUNCIONES******************************/
void setup(void){
    //CONFIG I&0
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 0;
    TRISB = 0;  
    TRISC2 = 0;
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 1;
    TRISD = 0; 
    TRISE = 0;
    PORTA = 0; 
    PORTB = 0; 
    PORTC = 0b00000100; 
    PORTD = 0; 
    PORTE = 0;            
    INTCONbits.TMR0IF = 0; //Interrupciones
    INTCONbits.RBIF = 0;
    INTCONbits.GIE = 1; //Habilitar interrupciones
    INTCONbits.PEIE = 1; 
    INTCONbits.T0IE = 1; //Interrupción del timer 0
    INTCONbits.RBIE = 0;
    PIR1bits.ADIF = 0; 
    OSCCONbits.IRCF0 = 1; //Configuración del oscilador (8MHz)
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    ADCON1 = 0;
    TMR0 = 216; //Donde comienza el timer 0
    OPTION_REG = 0b01010111; //Configuración de timer 0 y pull ups
    PIE1bits.RCIE = 1;  //Habilita EUSART Recieve Interrupt
    PIE1bits.TXIE = 1;  //Habilita EUSART Transmit Interrupt
    USARTcon();
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}

void LECT1(void){ //Para el primer puerto analógica
    Cen1 = v1/51; //Se crea bit de centena de 5V a 0V
    Dec1 = ((10*v1)/51-Cen1*10); //Se crea el bit de decena
    Un1  = (100*v1)/51-(Cen1*100+Dec1*10); //Se crea el bit de unidad
    AC1 = num_ascii(Cen1); //Se hace conversión de números a su forma ascii para que lea la LCD
    AD1 = num_ascii(Dec1);
    AU1 = num_ascii(Un1);
}
void LECT2(void){ //Se hace lo mismo para el segundo potenciómetro
    Cen2 = v2/51;
    Dec2 = (((100*v2)/51-(Cen2*100))/10);
    Un2 = (100*v2)/51-(100*Cen2+Dec2*10);
    AC2 = num_ascii(Cen2);
    AD2 = num_ascii(Dec2);
    AU2 = num_ascii(Un2);
}
void envio(void){   //Lectura de la terminal virtual
    toggleTX++;     //Siempre se irá aumentando este contador para que mande datos uno por uno
    if (toggleTX == 1){ //P
        TXREG = 0x50;
    }
    if (toggleTX == 2){ //O
        TXREG = 0x4F;
    }
    if (toggleTX == 3){ //T
        TXREG = 0x54;
    }
    if (toggleTX == 4){ //1
        TXREG = 0x31;
    }
    if (toggleTX == 5){ //:
        TXREG = 0x3A;
    }
    if (toggleTX == 6){ //spc
        TXREG = 0x20;
    }
    if (toggleTX == 7){ //centena potenciómetro 1
        TXREG = AC1;
    }
    if (toggleTX == 8){ //.
        TXREG = 0x2E;
    }
    if (toggleTX == 9){//decena potenciómetro 1
        TXREG = AD1;
    }
    if (toggleTX == 10){ //u P1
        TXREG = AU1;
    }
    if (toggleTX == 11){ //v
        TXREG = 0x76;
    }
    if (toggleTX == 12){ //,
        TXREG = 0x2C;
    }
    if (toggleTX == 13){ //spc
        TXREG = 0x20;
    }
    if (toggleTX == 14){ //P
        TXREG = 0x50;
    }
    if (toggleTX == 15){ //O
        TXREG = 0x4F;
    }
    if (toggleTX == 16){ //T
        TXREG = 0x54;
    }
    if (toggleTX == 17){ //2
        TXREG = 0x32;
    }
    if (toggleTX == 18){ //:
        TXREG = 0x3A;
    }
    if (toggleTX == 19){ //cent P2
        TXREG = AC2;
    }
    if (toggleTX == 20){ //.
        TXREG = 0x2E;
    }
    if (toggleTX == 21){ //dec P2
        TXREG = AD2;
    }
    if (toggleTX == 22){ //un P2
        TXREG = AU2;
    }
    if (toggleTX == 23){ //v
        TXREG = 0x76;
    }
    if (toggleTX == 24){ //brk
        TXREG = 13;
        toggleTX = 0;
    }
}
void CONTADOR(void){
    if (signo == 43){   //Si el signo es "+" (43 en código ascii)
        sum = 1;    //Se levantará la bandera de suma
    }
    
    if (signo == 13 && sum == 1){   //Si se apacha enter al estar la bandera de suma levantada
            sum = 0;
            PORTD++;                 //El contador se le sumará 1
    }
    
    if (signo == 45){  //Si el signo es -
        res = 1;
    }

    if(signo ==13 && res == 1){ //Y se apacha enter
        res = 0;
        PORTD--; //Se restará
    }
}