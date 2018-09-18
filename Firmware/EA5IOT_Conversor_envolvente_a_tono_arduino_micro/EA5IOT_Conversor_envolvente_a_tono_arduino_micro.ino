/*EA5IOT_Conversor_envolvente_a_tono_arduino_micro
    Copyright (C) 2018  EA5IOT

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
 
#include <TimerThree.h>

#define ADC_PIN               A3                                                // Entrada analogica
#define DAC_PIN                9                                                // Salida tono 500Hz

#define Puntos                20                                                // Puntos de la senoidal

uint8_t Conversor;
uint8_t Amplitud;
int16_t Nivel;
uint8_t Fase;

int8_t Tabla[Puntos];

void setup()
{
  ADMUX = 0x64;                                                                 // ADC en la pata A3, usando referencia AVCC, ajuste izquierdo y cogemos solo los 8bits de mayor peso
  ADCSRA = 0x83;                                                                // AD-converter on, interrupt disabled, prescaler = 8 Ftotal = 2Mhz
  ADCSRB = 0x80;                                                                // HIGH conversion rate   

  pinMode (DAC_PIN, OUTPUT);                                                    // PWM para el tono de salida

  TCCR1A = bit (COM1A1) | bit (WGM11);                                          // Toggle OC1A on Compare Match, ICR1 define el TOP
  TCCR1B = bit (WGM13) | bit (CS11);                                            // PWM phase correct, prescaler of 8
  ICR1 = 100;                                                                   // Frecuencia = 10Khz
  OCR1A = 50;                                                                   // Ancho de pulso del 50%

  for (Fase = 0; Fase < Puntos; Fase++)
  {
    Tabla[Fase] = 50*sin((2*PI/Puntos)*Fase);                                  // Senoidal de "Puntos"
  };
  Fase = 0;

  Timer3.initialize(100);                                                       // 100 microsegundos
  Timer3.attachInterrupt(Generar_Tono);                                         // Rutina para generar el tono de 500Hz
  Timer3.start();
}

void loop()
{
  Conversor = ADCH;                                                             // Se lee el valor de la conversion anteior
  ADCSRA = 0xC4;                                                                // Se habilita la siguiente conversion

  Amplitud = abs(Conversor - 127);                                              // Valor pico de la envolvente
}

void Generar_Tono()
{
  Nivel = Tabla[Fase] * Amplitud;                                               // Se calcula el nivel de la senoidal en función de la envolvente
  OCR1A = 50 + (Nivel >> 7);                                                    // Se genera un punto de la senoidal
  Fase++;                                                                       // Apuntamos al siguiente punto de la senoidal
  if (Fase >= Puntos) Fase = 0;                                                 // Recorremos los 20 puntos de la senoidal
}



