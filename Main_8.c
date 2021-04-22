/* UNIVERSIDAD DEL VALLE DE GUATEMALA
 * DEPARTAMENTO DE INGENIERIA ELCTRONICA & MECATRONICA
 * CURSO DE PROGRAMACION DE MICROCONTROLADORES
 * LABORATORIO No.8
 * 
 * File:   main7.c
 * Author: Selvin E. Peralta Cifuentes 
 *
 * Created on 20 de abril de 2021, 09:21 AM
 * Ultima Actualizacion:   21 de abril de 2021
 */
//-----------------------------------------------------------------------------------------------------------------------------------
// CONFIG1
//-----------------------------------------------------------------------------------------------------------------------------------
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)
//-----------------------------------------------------------------------------------------------------------------------------------
// CONFIG2
//-----------------------------------------------------------------------------------------------------------------------------------
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#include <stdint.h>             // TIPOS DE DATOS ESTANDAR Y OTROS.
#define _XTAL_FREQ 4000000
// DECLARACION DE VARIABLES
uint8_t V;
uint8_t V1;
uint8_t Unidad;
uint8_t Decena;
uint8_t Centena;
uint8_t Suitw;
//__________________________
// PROTOTIPOS DE FUNCIONES
void Setup(void);       // Llamamos las configuraciones de los pines 
int  Tabla(int V1);    // Llamamos la funcion tabla
void Decimal_CV(void);//  Llamamos la funcion de conversion a decimales 
//__________________________
//__________________________
// FUNCION DE INTERRUPCIONES
void __interrupt() isr(void){
//----------------------------- INTERRUPCION TMR0 ------------------------------    
    if(T0IF == 1) {       //Verificamos si la bandera del TMR0 es 1
        TMR0  = 236;      //Colocamos el valor a TMR0 para que funcione la interrupcion cada 5ms
        T0IF  = 0;        //Reiniciamos la bandera del TMR0
        PORTE = 0x00;     //Encendemos el Puerto E
        if(Suitw == 0){       //Verificamos la variable para poder realizar el suicheo 
            V1    = Unidad;   //Movemos el valor de la variable unidad a V
            PORTC = Tabla(V1);//Movemos el valor de la tabla a PORTD
            RE0   = 1;        //Volvemos 0 el pin del puerto E
            Suitw = 1;        //Cambiamos el valor de Suitw para poder controlar el suicheo 
            return;
        }    
        if(Suitw == 1){
            V1    = Decena;
            PORTC = Tabla(V1);
            RE1   = 1;
            Suitw = 2;
            return;
        }    
        if(Suitw == 2){
            V1    = Centena;
            PORTC = Tabla(V1);
            RE2   = 1;
            Suitw = 0;
            return;
        }
    }
//----------------------------- INTERRUPCION DEL ADC ----------------------------
    if(ADIF == 1){           //Verificamos si la bandera del IoCH es 1 
        if(ADCON0bits.CHS == 0){        //Verificamos si el pin esta precionado 
            PORTD = ADRESH;         //Aumentamos el Puerto C cuando se preciona el boton 
        }
        else{       //Verificamos si el pin esta precionado 
            V = ADRESH;        //Decrementamos el Puerto C cuando se preciona el boton
        }
        ADIF = 0;          //Limpiamos la bandera del IoCH
    }
}
//__________________________
// FUNCION PRINCIPAL (MAIN)
//__________________________
void main(void){
    Setup();
    __delay_us(50);
    ADCON0bits.GO = 1;
    while(1){
     Decimal_CV();
     if(ADCON0bits.GO == 0){ 
         if (ADCON0bits.CHS == 0){
             ADCON0bits.CHS = 1;
         }
         else{
             ADCON0bits.CHS = 0;
         }
         __delay_us(50);
         ADCON0bits.GO = 1;
     }
   }
     
}
//__________________________
//TABLA DE VALORES DE LOS DISPLAYS
int Tabla(int V1){
    int w;
    switch (V1){
        case 0:
        w = 0b00111111;
        break;
        case 1:
        w = 0b00000110;
        break;
        case 2:
        w = 0b01011011;
        break;
        case 3:
        w = 0b01001111;
        break;
        case 4:
        w = 0b01100110;
        break;
        case 5:
        w = 0b01101101;
        break;
        case 6:
        w = 0b01111101;
        break;
        case 7:
        w = 0b00000111;
        break;
        case 8:
        w = 0b01111111;
        break;
        case 9:
        w = 0b01101111;
        break;
        default:
        w = 0;    
    }
    return w;
}
//__________________________
// FUNCION DE DIVISION
void Decimal_CV(void){
     Centena =  V/100;   //Realizamos la operacion para encontrar la centana 
     Decena = (V - Centena*100)/10;//Realizamos la operacion para encontrar la decena 
     Unidad = (V - Centena*100 - Decena*10)/1;//Realizamos la operacion para encontrar la unidad
     return;
}
//__________________________
// FUNCION DE CONFIGURACION

void Setup(void){
//------------------- CONFIGURACION DE ENTRADAS Y SALIDAS ----------------------
    ANSEL  = 0b00000011;    // Configuracion a pines Digitale 
 //Configuramos que pines queremos como salidas o entradas 
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    TRISA = 0b00000011;
 //Limpiamos todos los puertes 
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
    PORTA = 0x00;
//---------------------- CONFIGURACION DE RELOJ A 4MHZ -------------------------
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
    OSCCONbits.SCS   = 1;
 //---------------------- CONFIGURACION DE ADC ------- -------------------------
    ADCON1bits.ADFM  = 0;
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    
    ADCON0bits.ADCS  = 1;   //Configuracion del reloj del modulo 
    ADCON0bits.CHS   = 0;   //Seleccionamos el canal donde querramos empezar
    __delay_us(100);
    ADCON0bits.ADON  = 1;   //Encendemos el modulo ADC
    __delay_us(50);
//-------------------------- CONFIGURACION DEL TMRO ----------------------------
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA  = 0;
    OPTION_REGbits.PS2  = 1;
    OPTION_REGbits.PS1  = 1;
    OPTION_REGbits.PS0  = 1;
//---------------------- CONFIGURACION DE INTERRUPCIONES -----------------------
    INTCONbits.GIE  = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE   = 1;
    INTCONbits.T0IE = 1;
    PIR1bits.ADIF   = 1;
}

