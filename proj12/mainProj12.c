#include <REG517A.H>

#define TH1_Init 204
#define BUFFER_SIZE 16
#define prev(x) (x - 1) % BUFFER_SIZE
#define next(x) (x + 1) % BUFFER_SIZE

#define CORRECAO 9
#define FrClk 12000000
#define FreqTimer0_emHz 100
#define TH0_Init ((65536 - (FrClk/(12*FreqTimer0_emHz)) + CORRECAO) >> 8)
#define TL0_Init ((65536 - (FrClk/(12*FreqTimer0_emHz)) + CORRECAO) & 0xFF)


unsigned char TxOcupado = 0;
unsigned long int valor_adc = 0;
unsigned char contador = 0;
char str[6];


typedef struct BufferCircular {
	unsigned char in;
	unsigned char out;
	unsigned char buff[16];
} BufferCircular;

unsigned char RecebeuString;
BufferCircular TxBuffer;

void serial_init(){
	SM0 = 0;	
	SM1 = 1;	
	SM20 = 0;
	PCON |= 0x80; 
	BD = 1; 
	REN0 = 1;
	ES0 = 1;
}

void timer0_init(){
	TR0 = 0;
	TMOD = (TMOD & 0xF0) | 0x01;
	
	TH0 = TH0_Init;
	TL0 = TL0_Init;
	
	ET0 = 1;
	TR0 = 1;

}

void adc_init(){
	ADM = 1;
	ADCON1 = 0;
	ADDATL = 0x40;

}

void read_adc(){
	unsigned int h,l;
	h = ADDATH << 2;
	l = ADDATL >> 6;
	
	valor_adc = h | l;
	
	valor_adc = (valor_adc*500)/1023;
	
	str[0] = (valor_adc/100) + '0';
	str[1] = ',';
	str[2] = ((valor_adc/10) % 10) + '0';
	str[3] = (valor_adc % 10) + '0';
	str[4] = '\n';
	str[5] = '$';

	
}

void buffer_init() {
	TxBuffer.in = TxBuffer.out = 0;
}

char TxBufferVazio() {
	return (TxBuffer.in == TxBuffer.out);
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
	
void SendString(char *s) {
	unsigned char i = 0;
	
	while (s[i] != '$') {
		SendChar(s[i]);
		i++;
	}

	SendChar(s[i]); // '$'

}

void ISR_Serial() interrupt 4 {
	if(TI0){
		TI0 = 0;
		if (TxBufferVazio() == 0){
			S0BUF = removeTxBuffer(); 
		}
		else TxOcupado = 0;
	}

}

void timer0_interrupt() interrupt 1{
	TR0 = 0;
	
	TL0 += TL0_Init;
	TH0 += TH0_Init + (unsigned char) CY;
	
	TR0 = 1;
	
	contador += 1;

}

void main() {
	serial_init();
	timer0_init();
	buffer_init();
	adc_init();
  EAL = 1; // Habilita interrupcoes globais
	
	while(1) {
		if(!BSY && contador > FreqTimer0_emHz){
			read_adc();
			SendString(str);
			contador = 0;
			}
		
  }
}
