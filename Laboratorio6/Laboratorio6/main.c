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

// Configuración de LEDs
#define LED_PORTB_MASK 0x3F  // B0-B5
#define LED_PORTD_MASK 0xC0   // D6-D7

// Prototipos de funciones
void UART_Init(unsigned int ubrr);
void UART_Transmit(unsigned char data);
unsigned char UART_Receive(void);
void show_on_leds(unsigned char value);

// Inicialización UART
void UART_Init(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Transmitir un carácter
void UART_Transmit(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

// Recibir un carácter
unsigned char UART_Receive(void) {
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

// Mostrar valor en LEDs (B0-B5 y D6-D7)
void show_on_leds(unsigned char value) {
	PORTB = value & LED_PORTB_MASK;
	PORTD = (PORTD & ~LED_PORTD_MASK) | (value & LED_PORTD_MASK);
}

int main(void) {
	// Inicializar UART
	UART_Init(MYUBRR);
	
	// Configurar LEDs
	DDRB = LED_PORTB_MASK;
	DDRD |= LED_PORTD_MASK;
	
	// Variable para controlar el modo
	unsigned char mode = 0; // 0 = Parte 1 (Transmisión), 1 = Parte 2 (Recepción)
	
	// Contador para la Parte 1
	unsigned char counter = 0;
	
	while(1) {
		if (mode == 0) {
			// Parte 1: Transmitir carácter y mostrar en LEDs
			UART_Transmit('A' + (counter % 26)); // Envía letras de la A a la Z
			
			show_on_leds(counter);
			counter++;
			
			_delay_ms(500);
			
			// Verificar si se recibió un carácter para cambiar de modo
			if (UCSR0A & (1 << RXC0)) {
				UART_Receive(); // Limpiar buffer
				mode = 1; // Cambiar a modo recepción
				UART_Transmit('\n'); // Nueva línea
				UART_Transmit('S'); UART_Transmit('w'); UART_Transmit('i');
				UART_Transmit('t'); UART_Transmit('c'); UART_Transmit('h');
				UART_Transmit('i'); UART_Transmit('n'); UART_Transmit('g');
				UART_Transmit(' '); UART_Transmit('t'); UART_Transmit('o');
				UART_Transmit(' '); UART_Transmit('R'); UART_Transmit('e');
				UART_Transmit('c'); UART_Transmit('e'); UART_Transmit('i');
				UART_Transmit('v'); UART_Transmit('e'); UART_Transmit(' ');
				UART_Transmit('M'); UART_Transmit('o'); UART_Transmit('d');
				UART_Transmit('e'); UART_Transmit('\n');
			}
			} else {
			// Parte 2: Recibir carácter y mostrar en LEDs
			if (UCSR0A & (1 << RXC0)) {
				unsigned char received = UART_Receive();
				show_on_leds(received);
				UART_Transmit(received); // Eco
				
				// Si se recibe 'T' cambiar a modo transmisión
				if (received == 'T' || received == 't') {
					mode = 0;
					UART_Transmit('\n'); // Nueva línea
					UART_Transmit('S'); UART_Transmit('w'); UART_Transmit('i');
					UART_Transmit('t'); UART_Transmit('c'); UART_Transmit('h');
					UART_Transmit('i'); UART_Transmit('n'); UART_Transmit('g');
					UART_Transmit(' '); UART_Transmit('t'); UART_Transmit('o');
					UART_Transmit(' '); UART_Transmit('T'); UART_Transmit('r');
					UART_Transmit('a'); UART_Transmit('n'); UART_Transmit('s');
					UART_Transmit('m'); UART_Transmit('i'); UART_Transmit('t');
					UART_Transmit(' '); UART_Transmit('M'); UART_Transmit('o');
					UART_Transmit('d'); UART_Transmit('e'); UART_Transmit('\n');
				}
			}
		}
	}
	
	return 0;
}

