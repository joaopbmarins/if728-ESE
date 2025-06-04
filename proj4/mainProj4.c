#include <REG51F.H>

#define TH1_Init 204

unsigned char ch = 0;

void timer1_init() {
	TR1 = 0;											// Desliga Timer 1
	TMOD = (TMOD & 0x0F) | 0x20;  // Timer 1 programado no Modo 2 (8 bits com autoreload)
	TH1 = TH1_Init;								// Programa valor de contagem do Timer 1
	TL1 = 0;											//
	ET1 = 0;											// Habilita Interrupcao do Timer 1
	TR1 = 1;											// Liga Timer 1
}

void serial_init(){
	SM0 = 0;	//modo 1
	SM1 = 1;	//modo 1
	SM2 = 0;
	PCON |= 0x80; // SMOD 1
	REN = 1; // habilita recepção serial
	ES = 1; // habilita interrupção serial

}

void ISR_Serial() interrupt 4 {
	if(RI){
		RI = 0;
		ch = SBUF;
		SBUF = ch + 1;
	}
	if(TI){
		TI = 0;
	}

}



void main() {  
  timer1_init(); // Inicializa o timer 0
	serial_init();
  EA = 1; // Habilita interrupções globais

	while(1) {
  }
}

