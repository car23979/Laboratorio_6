/*
 * Laboratorio6.c
 *
 * Created: 25/04/2025 12:08:10 p. m.
 * Author : David Carranza
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

// Variables globales
volatile unsigned char received_char = 0;
volatile uint8_t new_data_flag = 0;

// Inicialización UART
void UART_init(unsigned int ubrr) {
    // Configurar baud rate
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);  // RX y TX habilitados
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits, sin paridad
}

void UART_sendChar(char data) {
	while (!(UCSR0A & (1 << UDRE0)));  // Espera a que el buffer esté vacío
	UDR0 = data;
}