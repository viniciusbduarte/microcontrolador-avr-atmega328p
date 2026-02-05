/*
 * Bsp.h
 *
 * Created: 04/02/2026 12:19:35
 *  Author: Vinicius Duarte
 */ 


#ifndef BSP_H_
#define BSP_H_

void initHardware();
float readDistance();
void classificarCaixa(char categoria);
void mostrarContagem();
//void setPWM(uint8_t duty);

#endif /* BSP_H_ */