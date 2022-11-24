/*
 * ComunicacionSerial-Microfono.c
 *
 * Created: 23/11/2022 12:53:52 p. m.
 * Author : Martin
 */ 

#define F_CPU 2000000
#include <avr/io.h>
#include <util/delay.h>
#include <string.h> // Biblioteca que contiene constantes, macro y funciones

#define FOSC 2000000 // Clock Speed 2 Mhz
#define BAUD 9600

int analogRead(unsigned int numero_pin_analogo);
void Serial_USART(unsigned int baud);
void enviar_caracter(unsigned char caracter);
void enviar_cadena(char* cadena);
char unidades_mic, decenas_mic, centenas_mic, mil_mic;
char mil_luz,centenas_luz,decenas_luz,unidades_luz;


int main(void)
{
	int microfono=0;
	int luz=0;
	int contador=0;
	Serial_USART(9600);
	DDRB = 0b00000011; //B0 y B1 como salidas
	while (1)
	{
		microfono = analogRead(4);
		
		//Convertir a carácter resultado microfono
		unidades_mic = microfono%10 + 48;
		decenas_mic = (microfono/10)%10 + 48;
		centenas_mic = (microfono/100)%10 + 48;
		mil_mic = (microfono/1000) + 48;
		char cadena_texto_mic[5] = {mil_mic,centenas_mic,decenas_mic,unidades_mic};
		enviar_cadena("La intensidad de ruido es: ");
		enviar_cadena(cadena_texto_mic);
		enviar_cadena("\r\n");
		if(microfono<640){
			PORTB ^= (1<<PORTB1); // O-exclusiva bit a bit
			_delay_ms(1000);
		}
		
		//_delay_ms(100);
		
		//Convertir a carácter resultado Luz
		luz = analogRead(2);
		unidades_luz = luz%10 + 48;
		decenas_luz = (luz/10)%10 + 48;
		centenas_luz = (luz/100)%10 + 48;
		mil_luz = (luz/1000) + 48;
		char cadena_texto_luz[5] = {mil_luz,centenas_luz,decenas_luz,unidades_luz};
		enviar_cadena("La cantidad de luz es: ");
		enviar_cadena(cadena_texto_luz);
		enviar_cadena("\r\n"); // /t: Regulación Tab, /r: Return, /n: New Line

		if(luz <500){
			PORTB |= (1<<PORTB0);
		}
		else{
			PORTB &=~(1<<PORTB0);
		}
		_delay_ms(100);
		
		
		
		//enviar_caracter((unsigned char)(microfono/5));
		//enviar_cadena("\r\n");
		
	}
}

int analogRead(unsigned int numero_pin_analogo){
	
	ADMUX = 0x40 + numero_pin_analogo; //0x40 + 2 = 0x42
	ADCSRA = 0b11000100; //Activa la conversión Analogo Digital y pone preescalado Fosc/16
	//DIDR0 = (1<<2);
	while(ADCSRA&(1<<ADSC)); //Cuando la conversion termina el bit ADSC pasa a 0 y sale del ciclo
	ADCSRA&=~(1<<ADEN);//Desactivamos el conversor analogo digital
	
	return ADC;  //Devolvemos el valor convertido de analogo a digital.
	
}


void Serial_USART(unsigned int baud)
{
	/*Set baud rate */
	UBRR0 = ((FOSC/16)/baud)-1;
	/*Enable receiver and transmitter */
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8 bits de datos 1 bit de parada */
	UCSR0C=0b00000110;
}

void enviar_caracter(unsigned char caracter){
	while(!(UCSR0A&(1<<UDRE0)));    // mientras el registro UDR0 esté lleno espera
	UDR0 = caracter;            //cuando el el registro UDR0 está vacio se envia el caracter
	
}
void enviar_cadena(char* cadena){	 //cadena de caracteres de tipo char
	while(*cadena !=0x00){				//mientras el último valor de la cadena sea diferente
		//a el caracter nulo
		enviar_caracter(*cadena);//transmite los caracteres de cadena
		cadena++;//incrementa la ubicación de los caracteres en cadena
		//para enviar el siguiente caracter de cadena
	}
}

