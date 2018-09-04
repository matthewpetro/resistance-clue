
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

// Rotation values which determine the "up" direction for the LED display.
// Values reference the 4 pin header on the backpack which is used for
// connecting to the Arduino.
static const int ROTATION_HEADER_LEFT = 0;
static const int ROTATION_HEADER_BOTTOM = 1;
static const int ROTATION_HEADER_RIGHT = 2;
static const int ROTATION_HEADER_TOP = 3;

// Scroll speeds are defined using the number of milliseconds
// between screen refreshes. Whenever the screen refreshes,
// the text is moved left by one pixel.
static const int SCROLL_FAST = 100;
static const int SCROLL_MEDIUM = 150;
static const int SCROLL_SLOW = 200;

static const int DIAL0_INPUT = 0;
static const int DIAL1_INPUT = 1;

static const int I2C_ADDRESS = 0x70;
static const int X_POSITION = 0;
static const int Y_POSITION = 1;
// Size multiplier can be 1, 2, or 3
static const int SIZE_MULTIPLIER = 1;
// Brightness can be an integer in the range 0 to 15 inclusive
static const int BRIGHTNESS = 0;
static const int ROTATION = ROTATION_HEADER_RIGHT;
static const int SCROLL_SPEED = SCROLL_MEDIUM;

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

void setup()
{
	Serial.begin(9600);
	Serial.println("Resistance clue starting");

	matrix.begin(I2C_ADDRESS); // pass in the address
	matrix.setRotation(ROTATION);
	matrix.setBrightness(BRIGHTNESS);
	matrix.clear();
}

void loop()
{
	unsigned int dialInputs[2];
	dialInputs[0] = analogRead(DIAL0_INPUT);
	dialInputs[1] = analogRead(DIAL1_INPUT);

    unsigned int dialValues[2];
	dialValues[0] = translateDialInput(dialInputs[0]);
	dialValues[1] = translateDialInput(dialInputs[1]);

	char answerToDisplay = mapDialsToAnswer(dialValues);

	if (answerToDisplay == '\0') {
		displayText("Try again");
	}
	else {
		displayCharacter(answerToDisplay);
	}

	// Take the calculated value and display it.
	Serial.print("Values are ");
	Serial.print(dialValues[0]);
	Serial.print(" ");
	Serial.println(dialValues[1]);
}

unsigned int translateDialInput(unsigned int analogReading)
{
	// Translate ADC value from 0-1023 to 0-9.
	// This implements the proportion
	// input/1023 = val/9.
	// One "step" of the pot is about 113 ADC counts.
	// We're adding 65 (1/2 of 113) to the input value, so that the
	// input is in the middle of the window, rather than right at the edge, so values
	// are stable and solid.
	return (analogReading + 56) * 9 / 1023;
}

void displayCharacter(char character)
{
	matrix.clear();
	matrix.drawChar(X_POSITION, Y_POSITION, character, LED_ON, LED_OFF, SIZE_MULTIPLIER);
	matrix.writeDisplay();
}

void displayText(String text)
{
	const int CHARACTER_PIXEL_WIDTH = 6;
	matrix.setTextColor(LED_ON);
	matrix.setTextSize(SIZE_MULTIPLIER);
	matrix.setTextWrap(false);
	int textWidthInPixels = text.length() * CHARACTER_PIXEL_WIDTH;
	for (int8_t x = X_POSITION; x >= X_POSITION - textWidthInPixels; x--)
	{
		matrix.clear();
		matrix.setCursor(x, Y_POSITION);
		matrix.print(text);
		matrix.writeDisplay();
		delay(SCROLL_SPEED);
	}
}

char mapDialsToAnswer(unsigned int dialValues[]) {
	return '\0';
}