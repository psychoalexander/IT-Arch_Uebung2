/* Der ATmega328P l�uft mit einer Frequenz von 16 MHz */
#define F_CPU 16000000L

/* Einbinden der ben�tigten Bibliotheken */
#include <avr/io.h>
#include <util/delay.h>

int main() {

	/*	
	Alle Bits des DataDirectionRegister von Port D
	(DDRD) werden auf LOW gesetzt, sind somit als
	Eingang konfiguriert.
	In Bin�rschreibweise ist DDRD jetzt: 00000000 
	*/
	DDRD = 0x00;              

	/*	
	Das Bit an 8. Stelle (D7) wird auf Eins
	gesetzt -> Ausgang
	In Bin�rschreibweise ist DDRD jetzt: 10000000
	*/
	DDRD |= (1 << DDD7);

	/*
	Das Bit an 8. Stelle (D7) von PortD wird jetzt
	auf HIGH gesetzt. Die anderen Bits von Port D
	bleiben unver�ndert.
	In Bin�rschreibweise ist PortD jetzt: 1xxxxxxx
	*/
	PORTD |= (1 << DDD7);


	while(1) {
		// Das F�rderband dreht vorw�rts
		PORTD |= (1 << DDD7);
		
		// Das Programm wird um 1000 ms verz�gert
		_delay_ms(1000);

		// Das F�rderband wird angehalten
		PORTD &= ~(1 << DDD7);
		
		// Das Programm wird um 1000 ms verz�gert
		_delay_ms(1000);
	}
}