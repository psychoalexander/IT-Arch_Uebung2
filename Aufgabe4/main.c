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

	// Werkst�ck Counter, z�hlt wieviele WS sich auf dem Band befinden
	int wsCounter = 0;			
	
	// Flag, ob gerade ein WS sich im Sensorbereich am Bandanfang befindet
	int anfangBelegt = 0;

	// Flag, ob gerade ein WS sich im Sensorbereich am Bandende befindet
	int endeBelegt = 0;

	while(1) {
		// Pr�fen, ob der Endlagensensor am Bandanfang aktiv ist
		if( PIND & (1 << DDD2) ) {
			/* 
			Pr�fen ob anfangBelegt == 0 ist.  Falls ja, dann handelt
			es sich um eine steigende Flanke am Sensor
			*/
			if( anfangBelegt == 0) {
				// Werkst�ck ist neu am Anfang, daher anfangBelegt setzen
				anfangBelegt = 1;
				// wsCounter um eins hochz�hlen
				wsCounter++;
			}
		} else {
			// Es befindet sich kein WS im Anfangsbereich
			anfangBelegt = 0;
		}

		// Pr�fen, ob der Endlagensensor am Bandende aktiv ist
		if( PIND & (1 << DDD3) ) {
			// Es befindet sich ein WS im Endbereich, endeBelegt auf 1 setzen
			endeBelegt = 1;
		} else {
			/* 
			Falls endeBelegt == 1 ist, dann hat ein Werkst�ck
			den Sensorbereich am Bandende verlassen.
			endeBelegt wird wieder auf 0 gesetzt
			wsCounter wird eins heruntergez�hlt (falls wsCounter gr��er als 0 ist)
			*/
			if(endeBelegt == 1) {
				endeBelegt = 0;
				if(wsCounter > 0) wsCounter--;
			}
		}

		if(wsCounter > 0 && endeBelegt == 0) {
			// F�rderband vorw�rts drehen
			PORTD |= (1 << DDD7);
		} else {
			// F�rderband anhalten
			PORTD &= ~(1 << DDD7);
		}
	}
}