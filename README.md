# resistance-clue
Code for the Resistance clue for the 2018 Annual Arizona Treasure Hunt.

The clue requires Hunters to perform some resistor math. The clue also has a jackass answer.

Hunters use two dials to input the value of a resistor and then push a momentary button to test the answer. If the dial settings correspond to a correct or a jackass answer, an 8x8 LED matrix will display a letter. If they are incorrect, the matrix will scroll the text "TRY AGAIN".

The dials are 10-position rotary switches which have been configured as voltage dividers using identical resistors. When the button is pressed, the code reads the voltage from the dials and translates it into integers in the range 0 through 9, inclusive. It then uses the integers to determine what will be shown on the 8x8 matrix display.

# Good resources for building the hardware and software
- [https://www.arduino.cc/reference/en](https://www.arduino.cc/reference/en) - General Arduino programming reference  
- [https://learn.sparkfun.com/tutorials/rotary-switch-potentiometer-hookup-guide](https://learn.sparkfun.com/tutorials/rotary-switch-potentiometer-hookup-guide) - Tutorial on building a potentiometer out of SparkFun's 10-position rotary switch.  
- [https://learn.adafruit.com/adafruit-led-backpack/1-2-8x8-matrix](https://learn.adafruit.com/adafruit-led-backpack/1-2-8x8-matrix) - Tutorial on wiring up and using Adafruit's 8x8 matrix and backpack.  
- [https://learn.adafruit.com/adafruit-gfx-graphics-library](https://learn.adafruit.com/adafruit-gfx-graphics-library) - Documentation for Adafruit's GFX graphics library. This is the API used for programming the backpack and 8x8 matrix.  