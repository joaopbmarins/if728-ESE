#include <REG51F.H>

void main(){
  unsigned char auxP1 = 0;
  P1 = 0;

  while(1){
    auxP1 = 0;

    if(P2 & 0x02){
      auxP1 |= (P0 & 0xF0);
    }
    if(P2 & 0x01){
      auxP1 |= (P0 & 0x0F);
    }

    P1 = auxP1;
  }
  
}
