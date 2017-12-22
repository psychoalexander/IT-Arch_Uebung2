/* Der ATmega328P läuft mit einer Frequenz von 16 MHz */
#define F_CPU 16000000L

/* Einbinden der benötigten Bibliotheken */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Interrupt Counter
volatile uint32_t iCounter = 0;

// Inkremente Counter
volatile uint32_t nInkremente = 0;

/* 
Statuswort, gibt an in welchem Zustand sich die Anlage befindet
0 = Initial, kein Werkstück eingelegt, Band steht still.
1 = Werkstück am Bandanfang erkannt und exakt positioniert.
2 = Band dreht vorwärts, bis Position erkannt ist
...
*/
volatile uint8_t status = 0;

// Flag, ob der Lichtsensor ein schwarzes Inkrement erkennt.
volatile uint8_t sensorSchwarz = 0;

// Flag, ob gerade ein WS sich im Sensorbereich am Bandanfang befindet
volatile uint8_t anfangBelegt = 0;

// Flag, ob gerade ein WS sich im Sensorbereich am Bandende befindet
volatile uint8_t endeBelegt = 0;

void configureIO() {
	/*	
	Alle Bits des DataDirectionRegister von Port B
	(DDRB) werden auf LOW gesetzt, sind somit als
	Eingang konfiguriert.
	In Binärschreibweise ist DDRB jetzt: 00000000 
	*/
	DDRB = 0x00;

	/*	
	Das Bit an 1. Stelle (B0) wird auf Eins
	gesetzt -> Ausgang
	In Binärschreibweise ist DDRB jetzt: 00000001
	*/
	DDRB |= (1 << DDB0);

	/*	
	Alle Bits des DataDirectionRegister von Port D
	(DDRD) werden auf LOW gesetzt, sind somit als
	Eingang konfiguriert.
	In Binärschreibweise ist DDRD jetzt: 00000000 
	*/
	DDRD = 0x00;

	/*	
	Das Bit an 8. Stelle (D7) wird auf Eins
	gesetzt -> Ausgang
	In Binärschreibweise ist DDRD jetzt: 10000000
	*/
	DDRD |= (1 << DDD7);

	/*
	Das Bit an 8. Stelle (D7) von PortD wird jetzt
	auf HIGH gesetzt. Die anderen Bits von Port D
	bleiben unverändert.
	In Binärschreibweise ist PortD jetzt: 1xxxxxxx
	*/
	PORTD &= ~(1 << PORTD7);

	/*
	Das Bit an der 3. Stelle von Port D (DDD2) wird
	auf LOW gesetzt. Wenn diese Bit als Eingang definiert
	ist, wird dadurch der Pull-Up Widerstand deaktiviert. 
	In Binärschreibweise ist PortD jetzt: xxxxx0xx
	*/
	PORTD &= ~(1 << DDD2);

	/*
	Das Bit an der 4. Stelle von Port D (DDD3) wird
	auf LOW gesetzt. Wenn diese Bit als Eingang definiert
	ist, wird dadurch der Pull-Up Widerstand deaktiviert.
	In Binärschreibweise ist PortD jetzt: xxxx0xxx
	*/
	PORTD &= ~(1 << DDD3);

	/*
	Das Bit an der 6. Stelle von Port D (DDD5) wird
	auf LOW gesetzt. Wenn diese Bit als Eingang definiert
	ist, wird dadurch der Pull-Up Widerstand deaktiviert.
	In Binärschreibweise ist PortD jetzt: xx0xxxxx
	*/
	PORTD &= ~(1 << DDD5);
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

ISR(TIMER0_OVF_vect) {

}

void runInkrementSensorCheck() {
	if( PIND & (1 << PORTD5) ) {
		if(sensorSchwarz == 0) {
			// Flanke erkannt
			if(PORTD & (1 << PORTD7)) {
				// Bei Vorwärtsfahrt hochzählen
				nInkremente++;
			}
			if(PORTB & (1 << PORTB0)) {
				// Bei Rückwärtsfahrt runterzählen
				nInkremente--;
			}
		}
		sensorSchwarz = 1;
	}else {
		if(sensorSchwarz == 1) {
			// Flanke erkannt
			if(PORTD & (1 << PORTD7)) {
				// Bei Vorwärtsfahrt hochzählen
				nInkremente++;
			}
			if(PORTB & (1 << PORTB0)) {
				// Bei Rückwärtsfahrt runterzählen
				nInkremente--;
			}
		}
		sensorSchwarz = 0;
	}
}


int main() {

	configureIO();
	configureTimer();
	
	// Setze die Intterupt Register Bits
	sei();		

	while(1) {
		// Prüfen, ob Positionssensor inkrementiert wird
		runInkrementSensorCheck();

		// Prüfen, ob der Endlagensensor am Bandanfang aktiv ist
		if(status == 0) {
			// Werkstück am Bandanfang eingelegt
			if( (PIND & (1 << PIND2)) ) {
				// Fahre 2 Sekunden vorwärts
				PORTD |= (1 << PORTD7);
				_delay_ms(500);
				PORTD &= ~(1 << PORTD7);

				// Fahre Rückwärts bis zum Sensor
				PORTD &= ~(1 << PORTD7);
				PORTB |= (1 << PORTB0);

				while( !(PIND & (1 << PIND2)) ) {
					// Warten bis PIND2 auf HIGH geht
				}

				// Anhalten
				PORTD &= ~(1 << PORTD7);
				PORTB &= ~(1 << PORTB0);
				_delay_ms(2000);

				// Status auf 1 wechseln
				status = 1;
			}		
		}
		if(status == 1) {
			// Inkremente Counter auf 0 setzen
			nInkremente = 0;

			// Vorwärtsdrehen
			PORTD |= (1 << PORTD7);
			
			// Status auf 2 setzen
			status = 2;			
		}
		if(status == 2) {
			int mittelposition = 30;

			if(nInkremente >= mittelposition) {
				// Förderband anhalten
				PORTD &= ~(1 << PORTD7);
				_delay_ms(3000);
				status = 3;
				PORTD |= (1 << PORTD7);
			}
		}
		if(status == 3) {
			if(PIND & (1 << DDD3))
			{
				PORTD &= ~(1 << PORTD7);
				_delay_ms(2000);
				PORTB |= (1 << PORTB0); // faehert rueckwaerts
				status = 0;
			}
			
		}
	}
}