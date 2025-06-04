#include <REG51F.H>

#define TH1_Init 204
#define BUFFER_SIZE 16
#define prev(x) (x - 1) % BUFFER_SIZE
#define next(x) (x + 1) % BUFFER_SIZE

unsigned char TxOcupado = 0;

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
	REN = 1; // habilita recepcao serial
	ES = 1; // habilita interrupcao serial

}

typedef struct BufferCircular {
	unsigned char in;
	unsigned char out;
	unsigned char buff[16];
} BufferCircular;

unsigned char RecebeuString;
BufferCircular RxBuffer;
BufferCircular TxBuffer;

void buffer_init() {
	RxBuffer.in = RxBuffer.out = 0;
	TxBuffer.in = TxBuffer.out = 0;
}

char RxBufferVazio() {
	return (RxBuffer.in == RxBuffer.out);
}

char TxBufferVazio() {
	return (TxBuffer.in == TxBuffer.out);
}

void insertRxBuffer(char c) {
	unsigned char nextIn = next(RxBuffer.in);
	if (nextIn == RxBuffer.out) {
		RxBuffer.buff[prev(RxBuffer.in)] = '$';
		RecebeuString++;
		return;
	}
	RxBuffer.buff[RxBuffer.in] = c;
	RxBuffer.in = nextIn;
	if (c == '$') {
		RecebeuString++;
	}

	return;

}

char removeTxBuffer() {
	unsigned char c;
	
	c = TxBuffer.buff[TxBuffer.out];
	TxBuffer.out = next(TxBuffer.out);

	return c;
}
	
void SendChar(char c) {
	unsigned char nextIn = next(TxBuffer.in);
	if (nextIn == TxBuffer.out)	return;
	
	TxBuffer.buff[TxBuffer.in] = c;
	TxBuffer.in = nextIn;

	if (TxOcupado == 0) {
		TxOcupado = 1;
		TI = 1;
	}
}
	
char ReceiveChar() {
	unsigned char c;

	if (RxBufferVazio()) return '$';

	c = RxBuffer.buff[RxBuffer.out];
	RxBuffer.out = next(RxBuffer.out);

	return c;
}
	
void SendString(char *s) {
	unsigned char i = 0;
	
	while (s[i] != '$') {
		SendChar(s[i]);
		i++;
	}

	SendChar(s[i]); // '$'

}

void ReceiveString(char *s) {
	unsigned char i = 0;

	if (RecebeuString == 0) return;

	while (RxBuffer.buff[RxBuffer.out] != '$') {
		s[i] = ReceiveChar();
		i++;
	}

	s[i] = ReceiveChar(); // '$'

	RecebeuString--;
}	

void ISR_Serial() interrupt 4 {
	if(RI){
		RI = 0;
		insertRxBuffer(SBUF);
	}
	if(TI){
		TI = 0;
		if (TxBufferVazio() == 0){
			SBUF = removeTxBuffer(); 
		}
		else TxOcupado = 0;

	}

}

void main() {
	char s[16];
  timer1_init();
	serial_init();
	buffer_init();
  EA = 1; // Habilita interrupcoes globais
	
	while(1) {
		if(RecebeuString > 0){
			ReceiveString(s); // From Keyboard
			SendString(s); // To Serial
		}
  }
}

