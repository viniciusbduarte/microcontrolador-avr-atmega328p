/*
 * Bsp.c
 *
 * Created: 04/02/2026 12:09:54
 *  Author: Vinicius Duarte
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdbool.h>

// letras (exemplo)
#define SEG_G 0b11111010
#define SEG_M 0b00101010
#define SEG_P 0b11100110

uint16_t countG, countM, countP, countTotal;

  uint8_t seg[10] = {
	  0b01111110, // 0
	  0b00001100, // 1
	  0b10110110, // 2
	  0b10011110, // 3
	  0b11001100, // 4
	  0b11011010, // 5
	  0b11111010, // 6
	  0b00001110, // 7
	  0b11111110, // 8
	  0b11011110  // 9
  };
  
  

void initHardware() {
	// ===== HC-SR04 =====
	DDRB |= (1 << PB0);   // Trigger (PB0) saída
	DDRB &= ~(1 << PB1);  // Echo (PB1) entrada

	// ===== LEDs / Saídas P, M, G =====
	DDRB |= (1 << PB5);
	DDRB |= (1 << PB4);
	DDRB |= (1 << PB3);

	// ===== 7 segmentos / leds =====
	DDRD = 0b11111110;
	// ===== 7 segmentos / Digitos =====
	DDRC |= (1 << PC1);
	DDRC |= (1 << PC2);
	DDRC |= (1 << PC3);
	DDRC |= (1 << PC4);
	
	PORTC = 0b00000;
	
	// ===== Timer1 - Medição de tempo (HC-SR04) =====
	TCCR1A = 0;
	TCCR1B = 0;  // Timer parado inicialmente
	
}



void classificarCaixa(char categoria){
	if (categoria == 'G'){
		countG++;
		PORTB &= ~(1 << PB5);
		PORTB &= ~(1 << PB4);
		PORTB |= (1 << PB3);
	}
	else if (categoria == 'M'){
		countM++;
		PORTB &= ~(1 << PB5);
		PORTB |= (1 << PB4);
		PORTB &= ~(1 << PB3);
	}
	else if (categoria == 'P'){
		countP++;
		PORTB |= (1 << PB5);
		PORTB &= ~(1 << PB4);
		PORTB &= ~(1 << PB3);
	}
}


float readDistance() {
    uint16_t tempo_echo = 0;

    // 1. Gera o pulso de Trigger (10 microsegundos)
    PORTB &= ~(1 << PB0); 
    _delay_us(2);
    PORTB |= (1 << PB0);
    _delay_us(10);
    PORTB &= ~(1 << PB0);

    // 2. Espera o início do pulso Echo (ficar em nível alto)
    // Adicionar um timeout aqui seria ideal para evitar travamentos
    while (!(PINB & (1 << PB1)));

    // 3. Inicia o Timer1 (Prescaler de 8) 
    // Com 16MHz / 8 = 2MHz, cada "tick" do timer vale 0.5us
    TCNT1 = 0;           // Zera o contador
    TCCR1B |= (1 << CS11); 

    // 4. Espera o fim do pulso Echo (voltar para nível baixo)
    while (PINB & (1 << PB1));

    // 5. Para o Timer1
    tempo_echo = TCNT1;
    TCCR1B = 0; 

    /* * Cálculo da Distância:
     * Distância = (Tempo * Velocidade do Som) / 2
     * Com prescaler de 8, tempo em segundos = $TCNT1 \cdot \frac{8}{F\_CPU}$
     * $Distancia(cm) = \frac{TCNT1 \cdot 0.5 \cdot 0.0343}{2}$
     */
    return (float)tempo_echo * 0.008575;
}


void mostrarContagem() {
	//processo de teste
	char categoria = 'M';

	int d2 = 1;
	int d1 = 2;
	int d0 = 0;

	uint8_t tipo;

	if (categoria == 'G') {
		tipo = SEG_G;
	}
	else if (categoria == 'M') {
		tipo = SEG_M;
	}
	else {
		tipo = SEG_P;
	}

	// letra da categoria
	PORTC &= ~(1 << PC4);
	PORTD = tipo;
	_delay_ms(3);
	PORTC |= (1 << PC4);

	// dígito 2
	PORTC &= ~(1 << PC3);
	PORTD = seg[d2];
	_delay_ms(3);
	PORTC |= (1 << PC3);

	// dígito 1
	PORTC &= ~(1 << PC2);
	PORTD = seg[d1];
	_delay_ms(3);
	PORTC |= (1 << PC2);

	// dígito 0
	PORTC &= ~(1 << PC1);
	PORTD = seg[d0];
	_delay_ms(3);
	PORTC |= (1 << PC1);
}


