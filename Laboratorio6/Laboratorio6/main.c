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
	// Habilitar transmisor, receptor e interrupción por recepción
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	// Configurar formato: 8 bits de datos, 1 bit de parada
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Enviar un caracter
void UART_Transmit(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)));  // Espera a que el buffer esté vacío
	UDR0 = data;
}

// Función para enviar cadenas (Parte 1)
void enviar_cadena(char txt[]) {
	int i = 0;
	while (txt[i] != '\0') {
		UART_Transmit(txt[i]);
		i++;
	}
}

// Rutina de interrupción por recepción UART (Parte 2)
ISR(USART_RX_vect) {
	received_char = UDR0; // Leer dato recibido
	new_data_flag = 1;    // Activar flag
	
	// Mostrar en LEDs inmediatamente
	PORTB = received_char & 0x3F;       // B0-B5 (6 bits menos significativos)
	PORTD = (PORTD & 0x3F) | ((received_char << 2) & 0xC0); // D6-D7 (bits 6 y 7)
}

int main(void) {
	// Inicializar UART
	UART_Init(MYUBRR);
	
	// Configurar LEDs
	DDRB = 0x3F;    // B0-B5 como salidas
	DDRD |= (1 << PD6) | (1 << PD7); // D6 y D7 como salidas
	
	// Habilitar interrupciones globales
	sei();
	
	// Mensaje inicial
	enviar_cadena("Sistema UART listo\r\n");
	enviar_cadena("Parte 1: Envio automatico activo\r\n");
	enviar_cadena("Parte 2: Recepcion por interrupcion activa\r\n");
	
	unsigned char counter = 0;
	
	while(1) {
		// Parte 1: Envío periódico de caracteres
		UART_Transmit('A' + (counter % 26)); // Envia letras de la A a la Z
		enviar_cadena("\r\nEnviado: ");
		UART_Transmit('A' + (counter % 26));
		
		// Parte 2: Si se recibió nuevo dato, mostrar eco
		if(new_data_flag) {
			enviar_cadena("\r\nRecibido: ");
			UART_Transmit(received_char);
			new_data_flag = 0; // Resetear flag
		}
		
		_delay_ms(1000);
		counter++;
	}
}