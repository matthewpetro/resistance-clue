
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
static const int BUTTON_INPUT = 2;

static const int I2C_ADDRESS = 0x70;
static const int X_POSITION = 1;
static const int Y_POSITION = 1;
// Size multiplier can be 1, 2, or 3
static const int SIZE_MULTIPLIER = 1;
// Brightness can be an integer in the range 0 to 15 inclusive
static const int BRIGHTNESS = 0;
static const int ROTATION = ROTATION_HEADER_TOP;
static const int SCROLL_SPEED = SCROLL_FAST;

static const char RESPONSES[10][10] = {
    { '\0', '\0', '\0', '\0', 'G' , '\0', 'B' , '\0', '\0', '\0', },
	{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'K' , '\0', },
	{ 'J' , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', },
	{ '\0', 'O' , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', },
	{ '\0', '\0', '\0', '\0', '\0', 'A' , '\0', '\0', '\0', '\0', },
	{ '\0', '\0', '\0', '\0', 'R' , '\0', '\0', '\0', '\0', '\0', },
	{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'C' , '\0', },
	{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', },
	{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', },
	{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', },
};

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();
int previousButtonInput = HIGH;

void setup() {
	Serial.begin(9600);
	Serial.println("Resistance clue starting");

	matrix.begin(I2C_ADDRESS);
	matrix.setRotation(ROTATION);
	matrix.setBrightness(BRIGHTNESS);
	clearDisplay();

	pinMode(BUTTON_INPUT, INPUT_PULLUP);
}

void loop() {
	int currentButtonInput = digitalRead(BUTTON_INPUT);
	if (currentButtonInput != previousButtonInput) {
		if (currentButtonInput == HIGH) {
	    	clearDisplay();
	    }
	    else {
    		unsigned int dialValues[2];
			readDialValues(dialValues);
			char responseToDisplay = mapDialsToResponse(dialValues);
			displayResponse(responseToDisplay);
	    }
		previousButtonInput = currentButtonInput;
	}
}

void clearDisplay() {
	matrix.clear();
	matrix.writeDisplay();
}

void readDialValues(unsigned int dialValues[]) {
	unsigned int dialInputs[2];
	dialInputs[0] = analogRead(DIAL0_INPUT);
	dialInputs[1] = analogRead(DIAL1_INPUT);

	Serial.print("Dial 0 - ");
	dialValues[0] = translateDialInput(dialInputs[0]);
    Serial.print("  ");
	Serial.print("Dial 1 - ");
	dialValues[1] = translateDialInput(dialInputs[1]);
    Serial.println("");
}

void displayResponse(char responseToDisplay) {
    if (responseToDisplay == '\0') {
        scrollText("TRY AGAIN");
    }
	else {
        displayCharacter(responseToDisplay);
    }
}

unsigned int translateDialInput(unsigned int analogReading) {
	// Translate ADC value from 0-1023 to 0-9.
	// This implements the proportion
	// input/1023 = val/9.
	// One "step" of the pot is about 113 ADC counts.
	// We're adding 56 (1/2 of 113) to the input value, so that the
	// input is in the middle of the window, rather than right at the edge, so values
	// are stable and solid.
	float floatVal = (analogReading + 56) * float(9) / float(1023);
	unsigned int intVal = (analogReading + 56) * 9 / 1023;

    Serial.print("intVal: ");
	Serial.print(intVal);
	Serial.print(", floatVal: ");
	Serial.print(floatVal);
	Serial.print(", analog: ");
	Serial.print(analogReading);

	return intVal;
}

void displayCharacter(char character) {
	matrix.clear();
	matrix.drawChar(X_POSITION, Y_POSITION, character, LED_GREEN, LED_OFF, SIZE_MULTIPLIER);
	matrix.writeDisplay();
}

void scrollText(String text) {
	const int CHARACTER_PIXEL_WIDTH = 6;
	matrix.setTextColor(LED_RED, LED_OFF);
	matrix.setTextSize(SIZE_MULTIPLIER);
	matrix.setTextWrap(false);
	int textWidthInPixels = text.length() * CHARACTER_PIXEL_WIDTH;
	displayText(X_POSITION, Y_POSITION, text);
	delay(SCROLL_SPEED * 3);
	for (int x = X_POSITION; x >= X_POSITION - textWidthInPixels; x--) {
		displayText(x, Y_POSITION, text);
		delay(SCROLL_SPEED);
	}
}

void displayText(int xPos, int yPos, String text) {
	matrix.clear();
	matrix.setCursor(xPos, yPos);
	matrix.print(text);
	matrix.writeDisplay();
}

char mapDialsToResponse(unsigned int dialValues[]) {
	return RESPONSES[dialValues[0]][dialValues[1]];
}
