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
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

void UART_init(void) {
	UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
	UBRR0L = (unsigned char)(UBRR_VALUE);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);  // RX y TX habilitados
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits, sin paridad
}

void UART_sendChar(char data) {
	while (!(UCSR0A & (1 << UDRE0)));  // Espera a que el buffer esté vacío
	UDR0 = data;
}