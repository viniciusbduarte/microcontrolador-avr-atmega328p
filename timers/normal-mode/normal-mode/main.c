/*
 * normal-mode.c
 *
 * Created: 02/02/2026 14:22:44
 * Author : Vinicius Duarte
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER0_OVF_vect);

int main()
{
	DDRB |= 0b00100000;//PB5 como saída
	PORTB &= 0b11011111;//incia PB5 em LOW
	TCCR0A = 0b00000000;//TC0 em operação normal
	TCCR0B = 0b00000001;//TC0 sem prescaler. Overflow a cada 16us = 256/16MHz
	TIMSK0 = 0b00000001;//habilita a interrupção por estouro do TC0
	sei(); //habilita a chave de interrupção global
	
	while(1){}
}

ISR(TIMER0_OVF_vect) //interrupção do TC0
{
	PORTB ^= 0b00100000; //toggle 
}