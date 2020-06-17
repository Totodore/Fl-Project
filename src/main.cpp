#include <MIDI.h>

#define NUMBER_OF_PINS 8

//Index de chaque pin pour chaque chip
#define SER_PIN_INDEX 0
#define RCLK_PIN_INDEX 1
#define SCLK_PIN_INDEX 2

#define SLIDER_PIN 26

bool screensRegisters[3][NUMBER_OF_PINS];

const int screensPins[3][3] = {
	{10, 11, 12},
	{13, 14, 15},
	{16, 17, 18}
};
//potard, button
const int potarPins[3][2] = {
	{4, 5},
	{6, 7},
	{8, 9}
};
//slider, moteur
const int sliderPins[3][2] = {
	{20, 21},
	{22, 23},
	{24, 25}
};
/* Table de correspondance valeur -> états des segments de l'afficheur */
const byte LUT_ETATS_SEGMENTS[] = {
	0b00111111,
	0b00000110,
	0b01011011,
	0b01001111,
	0b01100110,
	0b01101101,
	0b01111101,
	0b00000111,
	0b01111111,
	0b01101111,
	0b01110111,
	0b01111100,
	0b00111001,
	0b01011110,
	0b01111001,
	0b01110001
};

//set all register pins to LOW
void clearRegisters(int index = -1)
{
	if (index == -1)
	{
		for (int j = 0; j < 5; j++)
		{
			for (int i = NUMBER_OF_PINS - 1; i >= 0; i--)
				screensRegisters[j][i] = LOW;
		}
	}
	else
	{
		for (int i = NUMBER_OF_PINS - 1; i >= 0; i--)
		{
			screensRegisters[index][i] = LOW;
		}
	}
}

//Set and display screensRegisters
//Only call AFTER all values are set how you would like (slow otherwise)
//Si pas de valeur spécifié ca écrit dans tous les chips
void writeRegisters(int index = -1)
{
	if (index == -1)
	{
		for (int j = 0; j < 5; j++)
		{
			digitalWrite(screensPins[index][RCLK_PIN_INDEX], LOW);

			for (int i = NUMBER_OF_PINS - 1; i >= 0; i--)
			{
				digitalWrite(screensPins[index][RCLK_PIN_INDEX], LOW);

				int val = screensRegisters[index][i];

				digitalWrite(screensPins[index][SER_PIN_INDEX], val);
				digitalWrite(screensPins[index][RCLK_PIN_INDEX], HIGH);
			}
			digitalWrite(screensPins[index][RCLK_PIN_INDEX], HIGH);
		}
	}
	else
	{
		digitalWrite(screensPins[index][RCLK_PIN_INDEX], LOW);

		for (int i = NUMBER_OF_PINS - 1; i >= 0; i--)
		{
			digitalWrite(screensPins[index][RCLK_PIN_INDEX], LOW);

			int val = screensRegisters[index][i];

			digitalWrite(screensPins[index][SER_PIN_INDEX], val);
			digitalWrite(screensPins[index][RCLK_PIN_INDEX], HIGH);
		}
		digitalWrite(screensPins[index][RCLK_PIN_INDEX], HIGH);
	}
}

//set an individual pin HIGH or LOW
void setRegisterPin(int chip_index, int index, bool value)
{
	screensRegisters[chip_index][index] = value;
}

struct MySettings : public midi::DefaultSettings
{
	static const long BaudRate = 115200;
};

/**
 * Envoi du volume : 0 à 50 pour les tracks, 0 à 100 pour le son.
 * Envoi de la position des tracks : 100, position de 0 à 50
 * */

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MySettings);

int volumeVal[50] = {80};

void writeScreen(int number, int chipIndex)
{
	/* Conversion chiffre -> états des segments */
	byte segments = LUT_ETATS_SEGMENTS[number];
	for (int i = 0; i < 7; i++)
	{
		setRegisterPin(chipIndex, i, !bitRead(segments, i));
	}
	writeRegisters(chipIndex);
}

void setup()
{
	MIDI.begin(MIDI_CHANNEL_OMNI);

	//On initialise tt les pins
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			pinMode(screensRegisters[i][j], OUTPUT);
		for (int j = 0; j < 2; j++)
			pinMode(potarPins[i][j], INPUT);
		pinMode(sliderPins[i][0], INPUT);
		pinMode(sliderPins[i][1], OUTPUT);
	}
	pinMode(SLIDER_PIN, INPUT);
}

void loop()
{
	//Volume recuperation
	if (MIDI.read())
	{
		volumeVal[MIDI.getData1()] = MIDI.getData2();
		MIDI.sendControlChange(MIDI.getData1(), MIDI.getData2() + 10, 1);
	}

	//Volume send
	// MIDI.sendControlChange(0, 60, 1);
	// MIDI.sendControlChange(1, 50, 1);
	// MIDI.sendControlChange(2, 40, 1);
	// MIDI.sendControlChange(3, 30, 1);
	// MIDI.sendControlChange(4, 20, 1);
	// MIDI.sendControlChange(5, 10, 1);

	//Tracks position
	// for (int i = 0; i < 20; i++) {
	// 	MIDI.sendControlChange(100, i, 1);
	// 	sleep(2);
	// }
}