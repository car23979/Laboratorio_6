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

int main(void) {
    // Inicializar UART
    UART_Init(MYUBRR);
    
    // Configurar LEDs
    DDRB = LED_PORTB_MASK;
    DDRD |= LED_PORTD_MASK;
    
    // Mensaje inicial
    cadena("\r\nSistema UART inicializado\r\n");
    cadena("Modo Transmision: Enviando A-Z\r\n");
    cadena("Envie cualquier caracter para cambiar a Recepcion\r\n");
    cadena("Envie 'T' para volver a Transmision\r\n\r\n");
    
    unsigned char mode = 0; // 0 = Transmisión, 1 = Recepción
    unsigned char counter = 0;
    
    while(1) {
        if(mode == 0) {
            // Parte 1: Transmitir carácter y mostrar contador en LEDs
            UART_Transmit('A' + (counter % 26));
            show_on_leds(counter);
            counter++;
            
            _delay_ms(500);
            
            // Verificar si se recibió un carácter para cambiar de modo
            if(UCSR0A & (1 << RXC0)) {
                UART_Receive(); // Limpiar buffer
                mode = 1;
                cadena("\r\nModo Recepcion activo\r\n");
                cadena("LEDs muestran caracteres recibidos\r\n");
                cadena("Presione 'T' para volver a Transmision\r\n\r\n");
            }
        } 
        else {
            // Parte 2: Recibir carácter y mostrar en LEDs
            if(UCSR0A & (1 << RXC0)) {
                unsigned char received = UART_Receive();
                show_on_leds(received);
                UART_Transmit(received); // Eco
                
                // Cambiar a modo transmisión si se recibe 'T' o 't'
                if(received == 'T' || received == 't') {
                    mode = 0;
                    counter = 0; // Reiniciar contador
                    cadena("\r\nModo Transmision activo\r\n");
                    cadena("Enviando A-Z cada 500ms\r\n\r\n");
                }
            }
        }
    }
    
    return 0;
}