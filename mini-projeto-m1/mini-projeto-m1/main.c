/*
 * blink.c
 *
 * Created: 20/01/2026 21:51:01
 * Author : Vinicius Duarte
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdbool.h>


void initHardware();
float readDistance();
void classificarCaixa(char categoria);
void setPWM(uint16_t duty);

uint16_t countG, countM, countP, countTotal;
bool flagDistancia;
float distancia, ultimaDistancia;

int main(void) {
    initHardware();
	
    while (1) {        
		distancia = readDistance();
		
		flagDistancia = (fabs(distancia - ultimaDistancia) < 10) ? false : true;

		if (flagDistancia) {
			if (distancia < 100.0) {
				classificarCaixa('G');
				} else if (distancia < 250.0) {
				classificarCaixa('M');
				} else if (distancia < 350.0) {
				classificarCaixa('P');
			}
		}

		ultimaDistancia = distancia;
        
        _delay_ms(100); 
    }
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









void initHardware() {
	    DDRB |= (1 << PB0);  // Trigger (PB0) como saída
    DDRB &= ~(1 << PB1); // Echo (PB1) como entrada

    DDRC |= (1 << PC1);  // P
    DDRC |= (1 << PC2);  // M
	DDRC |= (1 << PC3);  // G
	
    // Configuração do Timer1: Normal Mode, sem prescaler inicialmente
    TCCR1A = 0;
	
	//configuracao do pwm aqui
	    DDRD |= (1 << PD6); // PD6 como saída (OC0A)

	    // Fast PWM, TOP = 0xFF
	    TCCR0A = (1 << WGM01) | (1 << WGM00);
	    
	    // Clear OC0A on compare match (modo não invertido)
	    TCCR0A |= (1 << COM0A1);

	    // Prescaler = 64
	    TCCR0B = (1 << CS01) | (1 << CS00);

	    OCR0A = 128; // Duty cycle = 50%
	
	
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