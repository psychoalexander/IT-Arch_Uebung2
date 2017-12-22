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
	PORTD &= ~(1 << PORTD7);

	/*
	Das Bit an der 2. Stelle von Port D (DDD2) wird
	auf LOW gesetzt. Wenn diese Bit als Eingang definiert
	ist, wird dadurch der Pull-Up Widerstand deaktiviert. 
	In Bin�rschreibweise ist PortD jetzt: xxxxx0xx
	*/
	PORTD &= ~(1 << DDD2);

	/*
	Das Bit an der 3. Stelle von Port D (DDD3) wird
	auf LOW gesetzt. Wenn diese Bit als Eingang definiert
	ist, wird dadurch der Pull-Up Widerstand deaktiviert.
	In Bin�rschreibweise ist PortD jetzt: xxxx0xxx
	*/
	PORTD &= ~(1 << DDD3);


	while(1) {		
		// Pr�fen, ob der Endlagensensor am Bandanfang aktiv ist
		if( PIND & (1 << DDD2) ) {
			// Der Sensor ist aktiv, daher das F�rderband vorw�rts drehen
			PORTD |= (1 << DDD7);
		} 

		// Pr�fen, ob der Endlagensensor am Bandende aktiv ist
		if( PIND & (1 << DDD3) ) {
			// Der Sensor ist aktiv, daher das F�rderband anhalten
			PORTD &= ~(1 << DDD7);
		}
	}
}