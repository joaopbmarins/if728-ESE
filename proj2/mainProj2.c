#include <REG51F.H>

#define UM_SEGUNDO 12658

enum {
  estouEm1,
  estouEm0,
  Espera1s
};

sbit P2_0 = P2^0;
sbit P2_1 = P2^1;

unsigned char auxP1 = 0;

void P1H() {
  static char estado = 0;
  static unsigned long int timer = 0;

  switch(estado) {
    case estouEm1:
      if (P2_1 == 0) {
        estado = estouEm0;
      }
      break;
    case estouEm0:
      if (P2_1 == 1) {
        timer = 0;
        auxP1 |= (P0 & 0xF0);
        estado = Espera1s;
      }
      break;
    case Espera1s:
      timer++;
      if (timer > UM_SEGUNDO) {
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
  static unsigned long int timer = 0;

  switch(estado) {
    case estouEm1:
      if (P2_0 == 0) {
        estado = estouEm0;
      }
      break;
    case estouEm0:
      if (P2_0 == 1) {
        timer = 0;
        auxP1 |= (P0 & 0x0F);
        estado = Espera1s;
      }
      break;
    case Espera1s:
      timer++;
      if (timer > UM_SEGUNDO) {
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
  while(1) {
    P1H();
    P1L();
  }
}
