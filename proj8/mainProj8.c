#include <REG517A.H>

void main(){
	long int i=0;
	
	while(1){
	
		P4 = 0xff;
		
		for(i=0;i<13500;i++);
		
		P4 = 0x00;
	
		for(i=0;i<13500;i++);
		
	}
	
}
