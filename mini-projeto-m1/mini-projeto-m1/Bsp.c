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

uint16_t countG, countM, countP, countTotal;


void initHardware() {
	// ===== HC-SR04 =====
	DDRB |= (1 << PB0);   // Trigger (PB0) saída
	DDRB &= ~(1 << PB1);  // Echo (PB1) entrada

	// ===== LEDs / Saídas P, M, G =====
	DDRC |= (1 << PC1);
	DDRC |= (1 << PC2);
	DDRC |= (1 << PC3);

	// ===== Timer1 - Medição de tempo (HC-SR04) =====
	TCCR1A = 0;
	TCCR1B = 0;  // Timer parado inicialmente

	// ===== Timer0 - PWM (Motor) =====
	DDRD |= (1 << PD6); // OC0A (PD6) como saída

	// Fast PWM (modo 3), TOP = 0xFF
	TCCR0A = (1 << WGM01) | (1 << WGM00);

	// PWM não invertido em OC0A
	TCCR0A |= (1 << COM0A1);

	// Prescaler = 64
	TCCR0B = (1 << CS01) | (1 << CS00);

	// Duty inicial = 50%
	OCR0A = 128;
}



void classificarCaixa(char categoria){
	if (categoria == 'G'){
		countG++;
		PORTC &= ~(1 << PC1);
		PORTC &= ~(1 << PC2);
		PORTC |= (1 << PC3);
	}
	else if (categoria == 'M'){
		countM++;
		PORTC &= ~(1 << PC1);
		PORTC |= (1 << PC2);
		PORTC &= ~(1 << PC3);
	}
	else if (categoria == 'P'){
		countP++;
		PORTC |= (1 << PC1);
		PORTC &= ~(1 << PC2);
		PORTC &= ~(1 << PC3);
	}
}


void setPWM(uint8_t duty){
	if (duty > 255) duty = 255;
	OCR0A = duty;
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