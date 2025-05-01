/*
 * Laboratorio6.c
 *
 * Created: 25/04/2025 12:08:10 p. m.
 * Author : David Carranza
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Configuración 
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

// Configuración de LEDs
#define LED_PORTB_MASK 0x3F  // B0-B5 (00111111)
#define LED_PORTD_MASK 0xC0   // D6-D7 (11000000)

// Prototipos de funciones
void UART_Init(unsigned int ubrr);
void UART_Transmit(unsigned char data);
void cadena(const char txt[]);
unsigned char UART_Receive(void);
void show_on_leds(unsigned char value);
uint16_t read_potenciometer(void);
void display_menu(void);
void my_itoa(uint16_t value, char txt[] buffer, uint8_t base); //Función itoa

// Inicialización UART
void UART_Init(unsigned int ubrr) {
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Función optimizada para enviar cadenas
void cadena(const char txt[]) {
    while(*txt != '\0') {
        while(!(UCSR0A & (1 << UDRE0))); // Esperar buffer vacío
        UDR0 = *txt++;
    }
}

// Transmitir un carácter
void UART_Transmit(unsigned char data) {
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

// Recibir un carácter
unsigned char UART_Receive(void) {
    while(!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

// Mostrar valor en LEDs (B0-B5 y D6-D7)
void show_on_leds(unsigned char value) {
    PORTB = value & LED_PORTB_MASK;
    PORTD = (PORTD & ~LED_PORTD_MASK) | (value & LED_PORTD_MASK);
}

// Leer potenciómetro en A0
uint16_t read_potentiometer(void) {
	ADMUX = (1 << REFS0) | (0 << MUX0); // Configura AVcc como referencia y ADC0 (A0)
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Iniciar conversión
	while(ADCSRA & (1 << ADSC)); // Esperar conversión
	return ADC; // Retornar valor entre 0 - 1023
}

// Mostrar Menú
void display_menu(void) {
	cadena("\r\n=== MENU PRINCIPAL ===");
	cadena("\r\n1. Leer Potenciometro (A0)");
	cadena("\r\n2. Enviar Caracter ASCII");
	cadena("\r\nSeleccione opción (1-2): ");
}

// Función itoa
void my_itoa(uint16_t value, char txt[] buffer, uint8_t base) {
	char* p = buffer;
	uint16_t shifter = value;
	
	do {
		*p++ = "0123456789ABCDEF"[shifter % base];
		shifter /= base;
	} while(shifter);
	
	*p = '\0';
	
	// Invertir la cadena
	char *start = buffer, *end = p-1;
	while(start < end) {
		char temp = *start;
		*start++ = *end;
		*end-- = temp;
	}
}

int main(void) {
    // Inicializar UART
    UART_Init(MYUBRR);
    
    // Configurar LEDs
    DDRB = LED_PORTB_MASK;
    DDRD |= LED_PORTD_MASK;
    
    // Configurar ADC
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    // Mensaje inicial
    cadena("\r\nSistema UART con menu interactivo\r\n");
        
    while(1) {
	    display_menu();
	    
	    unsigned char option = UART_Receive();
	    UART_Transmit(option); // Eco
	    
	    switch(option) {
		    case '1': {
			    uint16_t pot_value = read_potentiometer();
			    cadena("\r\nValor potenciometro: ");
			    
			    // Convertir valor a ASCII y enviar
			    char buffer[10];
			    my_itoa(pot_value, buffer, 10);
			    cadena(buffer);
			    
			    // Mostrar en LEDs (solo bits bajos)
			    show_on_leds(pot_value & 0xFF);
			    break;
		    }
			
			case '2': {
				cadena("\r\nIngrese caracter ASCII: ");
				unsigned char ascii_char = UART_Receive();
				
				cadena("\r\nCaracter enviado: ");
				UART_Transmit(ascii_char);
				cadena(" (Decimal: ");
				
				// Mostrar valor decimal
				char dec_buffer[4];
				my_itoa(ascii_char, dec_buffer, 10);
				cadena(dec_buffer);
				cadena(")");
				
				// Mostrar en LEDs
				show_on_leds(ascii_char);
				break;
			}
			 default:
			 cadena("\r\nOpcion no valida!");
			 break;
		 }
		 
		 _delay_ms(100); // Pequeña pausa antes de mostrar menú nuevamente
	
	 
	 return 0;
 }    