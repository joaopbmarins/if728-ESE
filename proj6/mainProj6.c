#include <REG517A.H>

#define TH1_Init 204
#define BUFFER_SIZE 16
#define prev(x) (x - 1) % BUFFER_SIZE
#define next(x) (x + 1) % BUFFER_SIZE

unsigned char TxOcupado = 0;

void serial_init(){
	SM0 = 0;	//modo 1
	SM1 = 1;	//modo 1
	SM20 = 0;
	PCON |= 0x80; // SMOD 1
	BD = 1; // habilita o baudrate
	REN0 = 1; // habilita recepcao serial
	ES0 = 1; // habilita interrupcao serial

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
	unsigned char nextin = next(RxBuffer.in);
	if (nextin == RxBuffer.out) {
		RxBuffer.buff[prev(RxBuffer.in)] = '$';
		RecebeuString++;
		return;
	}
	RxBuffer.buff[RxBuffer.in] = c;
	RxBuffer.in = nextin;
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
	unsigned char nextin = next(TxBuffer.in);
	if (nextin == TxBuffer.out)	return;
	
	TxBuffer.buff[TxBuffer.in] = c;
	TxBuffer.in = nextin;

	if (TxOcupado == 0) {
		TxOcupado = 1;
		TI0 = 1;
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
	if(RI0){
		RI0 = 0;
		insertRxBuffer(S0BUF);
	}
	if(TI0){
		TI0 = 0;
		if (TxBufferVazio() == 0){
			S0BUF = removeTxBuffer(); 
		}
		else TxOcupado = 0;

	}

}

void main() {
	char s[16];
	serial_init();
	buffer_init();
  EAL = 1; // Habilita interrupcoes globais
	
	while(1) {
		if(RecebeuString > 0){
			ReceiveString(s); // From Keyboard
			SendString(s); // To SeRI0al
		}
  }
}
