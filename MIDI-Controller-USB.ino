#include <MIDI.h>

// Define the pins for potentiometers and push buttons
const int potPins[] = {A0, A1};
const int buttonPins[] = {2, 3};
const int numPots = sizeof(potPins) / sizeof(potPins[0]);
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

// Define MIDI CC numbers for potentiometers
const int potCCs[] = {10, 11};

// Define MIDI CC numbers for buttons
const int buttonCCs[] = {20, 21};

// Define the MIDI channel
const int midiChannel = 1;  // MIDI channels are 1-16, but remember MIDI uses 0-based numbering internally

// Create MIDI object
MIDI_CREATE_DEFAULT_INSTANCE();

// Calibration data for potentiometers
int potMinValues[numPots] = {0}; // Default to 0
int potMaxValues[numPots] = {1023}; // Default to 1023

// Variables to store previous state for buttons
bool prevButtonStates[numButtons];
unsigned long lastDebounceTime[numButtons];
unsigned long debounceDelay = 50;

void setup() {
  // Set the MIDI baud rate
  MIDI.begin(MIDI_CHANNEL_OFF);
  
  // Set the pins as inputs
  for (int i = 0; i < numPots; ++i) {
    pinMode(potPins[i], INPUT);
  }
  for (int i = 0; i < numButtons; ++i) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    prevButtonStates[i] = HIGH;
  }
}

void loop() {
  readPotentiometers();
  readButtons();
  
  // Add some delay to avoid flooding MIDI bus
  delay(10);
}

void readPotentiometers() {
  // Read the analog inputs and map them to MIDI CC values
  for (int i = 0; i < numPots; ++i) {
    int potValue = analogRead(potPins[i]);
    potValue = constrain(potValue, potMinValues[i], potMaxValues[i]);
    int mappedValue = map(potValue, potMinValues[i], potMaxValues[i], 0, 127);
    MIDI.sendControlChange(potCCs[i], mappedValue, midiChannel);
  }
}

void readButtons() {
  // Read the digital inputs and send MIDI CC messages for buttons
  for (int i = 0; i < numButtons; ++i) {
    int buttonState = digitalRead(buttonPins[i]);
    
    // Debounce the button
    if (buttonState != prevButtonStates[i]) {
      lastDebounceTime[i] = millis();
    }
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (buttonState != prevButtonStates[i]) {
        prevButtonStates[i] = buttonState;
        if (buttonState == LOW) {
          MIDI.sendControlChange(buttonCCs[i], 127, midiChannel);
        } else {
          MIDI.sendControlChange(buttonCCs[i], 0, midiChannel);
        }
      }
    }
  }
}
