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
 #include "Bsp.h"

 bool caixaDetectada;
 float distancia, ultimaDistancia;

 int main(void) {
	 initHardware();
 
	 while (1) {
		 distancia = readDistance();
 
		 caixaDetectada = (fabs(distancia - ultimaDistancia) < 10) ? false : true;

		 if (caixaDetectada) {
			 if (distancia < 100.0) {
				 classificarCaixa('G');
			 } else if (distancia < 250.0) {
				 classificarCaixa('M');
			 } else if (distancia < 350.0) {
				 classificarCaixa('P');
			 }
		}
		mostrarContagem();		
		ultimaDistancia = distancia;
 
	}
 }
