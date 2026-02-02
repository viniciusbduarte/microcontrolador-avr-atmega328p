# ⏱️ Timers no ATmega328P — Guia Completo com Exemplos

Este README explica em detalhes o funcionamento dos **timers do ATmega328P**, incluindo teoria, registradores, modos de operação e exemplos práticos em C.

> Microcontrolador: ATmega328P (Arduino Uno, Nano, etc.)
> Clock de referência: 16 MHz

---

## 📌 1) O que é um Timer?

Um **timer** é um contador de hardware que incrementa automaticamente com o clock do microcontrolador.

Ele pode ser usado para:

* ⏱️ Medir tempo
* 🔁 Gerar interrupções periódicas
* ⚡ Gerar PWM
* 📏 Medir sinais externos
* 🏭 Controle industrial (esteiras, sensores, etc.)

---

## 🧩 2) Timers do ATmega328P

O ATmega328P possui 3 timers:

| Timer  | Bits    | Tipo       | Uso comum                       |
| ------ | ------- | ---------- | ------------------------------- |
| Timer0 | 8 bits  | Básico     | delays, interrupções, PWM       |
| Timer1 | 16 bits | Avançado   | medição precisa, servo, captura |
| Timer2 | 8 bits  | Assíncrono | RTC, PWM independente           |

---

## ⚙️ 3) Estrutura básica de um Timer

### 📍 Registradores principais (exemplo: Timer0)

| Registrador   | Função                         |
| ------------- | ------------------------------ |
| TCCR0A        | Configuração do modo e compare |
| TCCR0B        | Prescaler e modo               |
| TCNT0         | Valor atual do contador        |
| OCR0A / OCR0B | Valores de comparação          |
| TIMSK0        | Habilita interrupções          |
| TIFR0         | Flags de interrupção           |

---

## 🧮 4) Clock, Prescaler e Overflow

### ⏱️ Clock do microcontrolador

Se o clock for 16 MHz:

```
1 ciclo = 1 / 16.000.000 = 62,5 ns
```

### ⚙️ Prescaler

O prescaler divide o clock:

| Prescaler | Bits CS02..CS00 |
| --------- | --------------- |
| 1         | 001             |
| 8         | 010             |
| 64        | 011             |
| 256       | 100             |
| 1024      | 101             |

### ⏳ Tempo de incremento do timer

```
T_tick = Prescaler / F_CPU
```

### 🔁 Overflow

Para Timer de 8 bits:

```
Overflow = 256 contagens
```

Tempo de overflow:

```
T_overflow = 256 × T_tick
```

---

# 🟢 5) Modos de Operação dos Timers

Os timers do ATmega328P possuem 4 modos principais:

1. Normal Mode
2. CTC Mode (Clear Timer on Compare)
3. Fast PWM
4. Phase Correct PWM

---

# 🟢 MODO 1 — NORMAL MODE

## 📖 Conceito

O timer conta de 0 até o máximo e gera overflow.

```
0 → 255 → overflow → 0 → 255 → ...
```

## ⚙️ Configuração (Timer0)

```c
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER0_OVF_vect)
{
    PORTB ^= (1 << PB5);
}

int main(void)
{
    DDRB |= (1 << PB5);

    TCCR0A = 0x00;              // Normal mode
    TCCR0B = (1 << CS02) | (1 << CS00); // Prescaler 1024
    TIMSK0 = (1 << TOIE0);      // Enable overflow interrupt

    sei();

    while(1);
}
```

## ⏱️ Cálculo do tempo

Prescaler = 1024

```
T_tick = 1024 / 16MHz = 64 µs
T_overflow = 256 × 64 µs = 16,384 ms
```

---

# 🟡 MODO 2 — CTC MODE (Clear Timer on Compare)

## 📖 Conceito

O timer zera quando atinge o valor de comparação (OCR0A).

```
0 → OCR0A → 0 → OCR0A → ...
```

## ⚙️ Configuração (Timer0)

```c
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER0_COMPA_vect)
{
    PORTB ^= (1 << PB5);
}

int main(void)
{
    DDRB |= (1 << PB5);

    TCCR0A = (1 << WGM01); // CTC mode
    TCCR0B = (1 << CS02) | (1 << CS00); // Prescaler 1024

    OCR0A = 156; // valor de comparação

    TIMSK0 = (1 << OCIE0A); // Enable compare interrupt

    sei();

    while(1);
}
```

## ⏱️ Cálculo do tempo

```
T_tick = 64 µs
T_compare = (OCR0A + 1) × T_tick
T_compare ≈ 157 × 64 µs ≈ 10 ms
```

---

# 🔵 MODO 3 — FAST PWM

## 📖 Conceito

O timer gera PWM rápido (frequência alta).

## ⚙️ Configuração (Timer0, PWM em OC0A = PD6)

```c
int main(void)
{
    DDRD |= (1 << PD6); // OC0A como saída

    TCCR0A = (1 << WGM01) | (1 << WGM00); // Fast PWM
    TCCR0A |= (1 << COM0A1); // PWM não invertido
    TCCR0B = (1 << CS01); // Prescaler 8

    OCR0A = 128; // Duty cycle 50%

    while(1);
}
```

## 📊 Duty Cycle

```
Duty = OCR0A / 255
```

---

# 🟣 MODO 4 — PHASE CORRECT PWM

## 📖 Conceito

O timer conta para cima e para baixo (PWM simétrico).

```
0 → 255 → 0 → 255 → ...
```

## ⚙️ Configuração

```c
int main(void)
{
    DDRD |= (1 << PD6);

    TCCR0A = (1 << WGM00); // Phase Correct PWM
    TCCR0A |= (1 << COM0A1);
    TCCR0B = (1 << CS01);

    OCR0A = 128;

    while(1);
}
```

---

# 🧠 6) Timer1 (16 bits) — Diferenças

Timer1 possui:

* 16 bits → 65536 contagens
* Input Capture (ICP1)
* PWM avançado

Exemplo básico:

```c
TCCR1A = 0x00;
TCCR1B = (1 << CS12); // Prescaler 256
TCNT1 = 0;
```

---

# 🧮 7) Fórmulas importantes

### ⏱️ Frequência do timer

```
F_timer = F_CPU / Prescaler
```

### ⏳ Período de interrupção
qua
```
T = (TOP + 1) × Prescaler / F_CPU
```

Onde:

* TOP = 255 (8 bits) ou OCRxA (CTC)

---
