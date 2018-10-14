
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
// Delay in milliseconds between pixel shifts during text scrolling
static const int SCROLL_SPEED = 50;

// Set up a 2 dimensional array to store the mapping of dial inputs
// to responses. This approach works well because the dial inputs
// are zero indexed just like arrays. The dial on the left corresponds
// to the rows below and the dial on the right corresponds to the columns.
static const char RESPONSES[10][10] = {
                                                                                 // LEFT DIAL
																	             //    |
																				 //   \ /
																				 //    '
            	{ '\0', '\0', '\0', '\0', 'G' , '\0', 'B' , '\0', '\0', '\0', }, // BLACK
	        	{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'K' , '\0', }, // BROWN
	        	{ 'J' , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', }, // RED
	        	{ '\0', 'O' , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', }, // ORANGE
	        	{ '\0', '\0', '\0', '\0', '\0', 'A' , '\0', '\0', '\0', '\0', }, // YELLOW
	        	{ '\0', '\0', '\0', '\0', 'R' , '\0', '\0', '\0', '\0', '\0', }, // GREEN
	        	{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 'C' , '\0', }, // BLUE
	        	{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', }, // VIOLET
	        	{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', }, // GRAY
            	{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', }, // WHITE
// RIGHT DIAL ->   BLK   BRN   RED   ORNG  YLOW  GRN   BLUE  VLET  GRY   WHT
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
	// The main portion of the code will only execute if the button
	// input has changed since the last time through the loop() function.
	if (currentButtonInput != previousButtonInput) {
		// Due to the use of the pullup  resistor, the button input
		// reads as HIGH when the button is not pressed.
		if (currentButtonInput == HIGH) {
	    	clearDisplay();
	    }
	    else {
    		unsigned int dialValues[2];
			readDialValues(dialValues);
			char responseToDisplay = mapDialsToResponse(dialValues);
			displayResponse(responseToDisplay);
	    }
	}
	previousButtonInput = currentButtonInput;
}

void clearDisplay() {
	matrix.clear();
	matrix.writeDisplay();
}

void readDialValues(unsigned int dialValues[]) {
	unsigned int dialInput0 = analogRead(DIAL0_INPUT);
	unsigned int dialInput1 = analogRead(DIAL1_INPUT);

	Serial.print("Dial 0 - ");
	dialValues[0] = translateDialInput(dialInput0);
    Serial.print("  ");
	Serial.print("Dial 1 - ");
	dialValues[1] = translateDialInput(dialInput1);
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
	float translatedValue = (analogReading + 56) * float(9) / float(1023);

    Serial.print("int value: ");
	Serial.print((unsigned int)translatedValue);
	Serial.print(", float value: ");
	Serial.print(translatedValue);
	Serial.print(", analog reading: ");
	Serial.print(analogReading);

	return (unsigned int)translatedValue;
}

void displayCharacter(char character) {
	matrix.clear();
	matrix.drawChar(X_POSITION, Y_POSITION, character, LED_GREEN, LED_OFF, SIZE_MULTIPLIER);
	matrix.writeDisplay();
}

void scrollText(String text) {
	matrix.setTextColor(LED_RED, LED_OFF);
	matrix.setTextSize(SIZE_MULTIPLIER);
	matrix.setTextWrap(false);
	displayText(X_POSITION, Y_POSITION, text);
	// Show the first characters of the text for double the delay time between
	// pixels shifts during scolling. This gives users just enough time to
	// start reading before the text scrolls.
	delay(SCROLL_SPEED * 2);
	const int CHARACTER_PIXEL_WIDTH = 6;
	int textWidthInPixels = text.length() * CHARACTER_PIXEL_WIDTH;
	// Within the for loop, the X position will be decremented until it's
	// far enough to the left that the text has been completely shifted
	// off the screen.
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
