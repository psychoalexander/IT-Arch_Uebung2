/* Der ATmega328P läuft mit einer Frequenz von 16 MHz */
#define F_CPU 16000000L

/* Einbinden der benötigten Bibliotheken */
#include <avr/io.h>
#include <util/delay.h>


void configureIO() {

	/*	
	Alle Bits des DataDirectionRegister von Port B
	(DDRB) werden auf LOW gesetzt, sind somit als
	Eingang konfiguriert.
	In Binärschreibweise ist DDRB jetzt: 00000000 
	*/
	DDRB = 0x00;

	/*	
	Das Bit an 2. Stelle (B1) wird auf Eins
	gesetzt -> Ausgang
	Schleichgang
	In Binärschreibweise ist DDRB jetzt: 00000010
	*/
	DDRB |= (1 << DDB1);

	/*
	Das Bit an 2. Stelle (B1) von Port B wird jetzt
	auf HIGH gesetzt. Die anderen Bits von Port B
	bleiben unverändert.
	In Binärschreibweise ist Port B jetzt: xxxxxx1x
	*/
	PORTB |= (1 << PORTB1);

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
	Das Bit an 7. Stelle (D6) wird auf Eins
	gesetzt -> Ausgang
	Geschwindigkeitsausgang
	In Binärschreibweise ist DDRD jetzt: 11000000
	*/
	DDRD |= (1 << DDD6);

	/*
	Das Bit an 8. Stelle (D7) von PortD wird jetzt
	auf HIGH gesetzt. Die anderen Bits von Port D
	bleiben unverändert.
	In Binärschreibweise ist PortD jetzt: 1xxxxxxx
	*/
	PORTD &= ~(1 << PORTD7);

	/*
	Das Bit an der 2. Stelle von Port D (DDD2) wird
	auf LOW gesetzt. Wenn diese Bit als Eingang definiert
	ist, wird dadurch der Pull-Up Widerstand deaktiviert. 
	In Binärschreibweise ist PortD jetzt: xxxxx0xx
	*/
	PORTD &= ~(1 << DDD2);

	/*
	Das Bit an der 3. Stelle von Port D (DDD3) wird
	auf LOW gesetzt. Wenn diese Bit als Eingang definiert
	ist, wird dadurch der Pull-Up Widerstand deaktiviert.
	In Binärschreibweise ist PortD jetzt: xxxx0xxx
	*/
	PORTD &= ~(1 << DDD3);
}

void configurePWM() {
	/*
	Timer Counter Control Register 0 A
	Den "mode of operation" auf FastPWM setzen
	*/
	TCCR0A |= (1<<WGM00 | 1<<WGM01);

	/*
	Timer Counter Control Register 0 B
	Die Frequenz wird die Grundfrequenz ohne Prescaler
	verwendet. Das heißt 16 MHz
	*/
	TCCR0B |= (1<<CS01);

	/*
	Mittels OCR0A kann das Schaltverhältnis angegeben werden.
	0x80 entspricht 50%
	*/
	OCR0A = 0x80;

	/* 
	Timer Counter Control Register 0 A
	Das Bit COM0A1 aktiviert die PWM
	*/
	TCCR0A |= 1<<COM0A1;
}

int main(void)
{
	configureIO();
	//configurePWM();

	// Das Förderband dreht vorwärts
	PORTD |= (1 << PORTD7);

	PORTB |= (1 << PORTB1); // Schleichgang aktiviert

    while (1) 
    {	
		/*
		TODO: Implementieren
		Analogausgang HIGH
		x ms warten
		Analogausgang LOW
		y ms warten
		*/
		
		PORTD |= (1 << DDD6);
		_delay_ms(1);
		PORTD &= ~(1 << DDD6);
		_delay_ms(5);
    }
}

