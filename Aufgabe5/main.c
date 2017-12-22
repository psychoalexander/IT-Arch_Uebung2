/* Der ATmega328P l�uft mit einer Frequenz von 16 MHz */
#define F_CPU 16000000L

/* Einbinden der ben�tigten Bibliotheken */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Interrupt Counter
volatile uint32_t iCounter = 0;

// Werkst�ck Counter, z�hlt wieviele WS sich auf dem Band befinden
volatile uint8_t wsCounter = 0;

// Flag, ob gerade ein WS sich im Sensorbereich am Bandanfang befindet
volatile uint8_t anfangBelegt = 0;

// Flag, ob gerade ein WS sich im Sensorbereich am Bandende befindet
volatile uint8_t endeBelegt = 0;

void configureIO() {
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
}

void configureTimer() {
	/*
	Timer Counter Control Register 0 A (TCCR0A)
	Register auf 0 setzen (default)
	Weiter Informationen siehe ATmega328P Handbuch
	*/
	TCCR0A = 0x00;
	/*
	Timer Counter Control Register 0 B (TCCR0B)
	Register auf 0 setzen (default)
	Die drei Bits CS00, CS01 und CS02 konfigurieren den Prescaler
	*/
	TCCR0B = 0x00;
	TCCR0B &= ~(1 << CS02);
	TCCR0B &= ~(1 << CS01);
	TCCR0B |= (1 << CS00);

	/*
	TC0 Interrupt Mask Register (TIMSK0)
	Das Bit TOIE aktiviert den Overflow Interrupt
	*/
	TIMSK0 = 0x00;
	TIMSK0 |= (1 << TOIE0);
}

/*
	TIMER0_OVF_vect ist der Interrupt Vector f�r einen
	Overflow von Timer 0

	ISR ist die Interrupt Service Routine
*/
ISR(TIMER0_OVF_vect) {
	/* 
	Falls das F�rderband vorw�rts dreht, dann iCounter eins hochz�hlen
	*/
	if (PIND & (1 << DDD7)) iCounter++;

	/*
	Clock von ATmega328P ist 16 MHz
	Der Timer kann 8 Bit (256 Werte) speichern bevor er �berlauft

	Der �berlauf ereignet sich demnach mit einer Frequenz
	f = 16 MHz / 256
	f = 62,5 KHz

	312500 �berl�ufe entsprechen 5 s
	*/
	if(iCounter > 312500) {
		if(wsCounter > 0) {
			wsCounter = 0;
		}
		iCounter = 0;
	}
}

int main() {

	configureIO();
	configureTimer();
	
	// Setze die Intterupt Register Bits
	sei();		

	while(1) {
		// Pr�fen, ob der Endlagensensor am Bandanfang aktiv ist
		if( PIND & (1 << DDD2) ) {
			/* 
			Pr�fen ob anfangBelegt == 0 ist.  Falls ja, dann handelt
			es sich um eine steigende Flanke am Sensor
			*/
			if(anfangBelegt == 0) {
				// Werkst�ck ist neu am Anfang, daher anfangBelegt setzen
				anfangBelegt = 1;
				// wsCounter um eins hochz�hlen
				wsCounter++;
				// Interrupt Counter auf 0 setzen
				iCounter = 0;
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
			wsCounter wird eins heruntergez�hlt (falls wsCounter > 0)
			iCounter auf 0 zur�cksetzen
			*/
			if (endeBelegt == 1)
			{
				endeBelegt = 0;
				if(wsCounter > 0) wsCounter--;
			}
		}

		/*
		Falls sich ein Werkst�ck auf dem Band befindet (wsCounter > 0)
		und kein Werkst�ck im Endbereich ist (endeBelegt == 0), dann
		soll das F�rderband vorw�rts drehen
		*/
		if( wsCounter > 0 && endeBelegt == 0 ) {
			// F�rderband vorw�rts drehen
			PORTD |= (1 << DDD7);
		} else {
			// F�rderband anhalten
			PORTD &= ~(1 << DDD7);
		}
	}
}