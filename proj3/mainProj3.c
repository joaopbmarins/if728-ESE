#include <REG51F.H>

#define CORRECAO 9
#define FrClk 12000000
#define FreqTimer0_emHz 100
#define TH0_Init ((65536 - (FrClk/(12*FreqTimer0_emHz)) + CORRECAO) >> 8)
#define TL0_Init ((65536 - (FrClk/(12*FreqTimer0_emHz)) + CORRECAO) & 0xFF)

enum {
	estouEm1,
	estouEm0,
  Espera1s
};

sbit P2_0 = P2^0;
sbit P2_1 = P2^1;
unsigned char auxP1 = 0;
unsigned char timerP1H = 0;
unsigned char timerP1L = 0;


void timer0_init() {
	TR0 = 0;											// Desliga Timer 0
	TMOD = (TMOD & 0xF0) | 0x01;  // Timer 0 programado no Modo 1 (16 bits)
	TH0 = TH0_Init;								// Programa valor de contagem do Timer 0
	TL0 = TL0_Init;								//
	ET0 = 1;											// Habilita Interrupcao do Timer 0
	TR0 = 1;											// Liga Timer 0
}

void timer0_int() interrupt 1 using 2 {
	TR0 = 0;															// Desliga Timer 0
	TL0 += TL0_Init;											// Faz recarga da contagem do Timer 0
	TH0 += TH0_Init + (unsigned char) CY;	// 
	TR0 = 1; 															// Liga Timer 0

  timerP1H++;
  timerP1L++;
}


void P1H() {
  static char estado = 0;

  switch(estado) {
    case estouEm1:
      if (P2_1 == 0) {
        estado = estouEm0;
      }
      break;
    case estouEm0:
      if (P2_1 == 1) {
        timerP1H = 0;
        auxP1 |= (P0 & 0xF0);
        estado = Espera1s;
      }
      break;
    case Espera1s:
      if (timerP1H > FreqTimer0_emHz) {
        auxP1 &= 0x0F;
        if (P2_1 == 0){
          estado = estouEm0;
        } else if (P2_1 == 1) {
          estado = estouEm1;
        }
      }

      break;
    default: break;
  }

  P1 = auxP1;
}

void P1L() {
  static char estado = 0;

  switch(estado) {
    case estouEm1:
      if (P2_0 == 0) {
        estado = estouEm0;
      }
      break;
    case estouEm0:
      if (P2_0 == 1) {
        timerP1L = 0;
        auxP1 |= (P0 & 0x0F);
        estado = Espera1s;
      }
      break;
    case Espera1s:
      if (timerP1L > FreqTimer0_emHz) {
        auxP1 &= 0xF0;
        if (P2_0 == 0){
          estado = estouEm0;
        } else if (P2_0 == 1) {
          estado = estouEm1;
        }
      }

      break;
    default: break;
  }

  P1 = auxP1;
}

void main() {
  P1 = 0x00;
  
  timer0_init(); // Inicializa o timer 0
  EA = 1; // Habilita interrupções globais

	while(1) {
    P1H();
    P1L();
  }
}
