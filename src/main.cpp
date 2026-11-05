#include <Arduino.h>
#include <BleKeyboard.h>

// Create the BLE keyboard object used to send key presses to the host device.
BleKeyboard bleKeyboard;

// 4x4 membrane keypad layout:
//   1  2  3  A
//   4  5  6  B
//   7  8  9  C
//   *  0  #  D
//
// Connect the keypad's 8-pin ribbon in this order:
//   R1, R2, R3, R4, C1, C2, C3, C4
//
// The row and column pin arrays define the ESP32 GPIO mapping for the keypad.
// Update these values if the physical wiring differs from the current board layout.
constexpr uint8_t ROW_PINS[4] = {13, 12, 14, 27};
constexpr uint8_t COL_PINS[4] = {26, 25, 33, 32};

// The key map matches each row and column position to the character or action key.
// Letters A-D are assigned as special navigation keys in the main loop.
constexpr char KEYMAP[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Short debounce delay so a single press is not interpreted as repeated key events.
constexpr unsigned long DEBOUNCE_MS = 35;

// Scan the keypad matrix and return the currently pressed key, or '\0' when no key is pressed.
// The row lines are driven one at a time while all rows are briefly set HIGH between scans.
// A pressed key is detected when one of the column inputs reads LOW.
char scanKeypad() {
    // Rows idle HIGH. Pull one row LOW at a time and look for a LOW column.
    for (uint8_t row = 0; row < 4; ++row) {
        // Ensure all rows are released before testing the active row.
        for (uint8_t r = 0; r < 4; ++r) {
            digitalWrite(ROW_PINS[r], HIGH);
        }

        // Activate the current row and wait a tiny amount for the signal to settle.
        digitalWrite(ROW_PINS[row], LOW);
        delayMicroseconds(5);

        // Check each column for a pressed switch in the selected row.
        for (uint8_t col = 0; col < 4; ++col) {
            if (digitalRead(COL_PINS[col]) == LOW) {
                return KEYMAP[row][col];
            }
        }
    }

    // No key is currently being pressed.
    return '\0';
}

// Called once when the ESP32 boots. Set up GPIO pins, serial debug output,
// and initialize the Bluetooth keyboard service that will send keystrokes.
void setup() {
    // Start the USB serial monitor for debugging and status messages.
    Serial.begin(115200);
    delay(200);

    // Configure all keypad rows as outputs and keep them idle HIGH.
    for (uint8_t row = 0; row < 4; ++row) {
        pinMode(ROW_PINS[row], OUTPUT);
        digitalWrite(ROW_PINS[row], HIGH);
    }

    // Configure all keypad columns as inputs with pull-up resistors.
    // A pressed key pulls the corresponding column LOW.
    for (uint8_t col = 0; col < 4; ++col) {
        pinMode(COL_PINS[col], INPUT_PULLUP);
    }

    // Give the user a quick serial startup message to confirm the board is alive.
    Serial.println();
    Serial.println("ESP32 4x4 keypad ready.");
    Serial.println("Press a key; its character will appear below.");
    Serial.println("Starting BLE work!");

    // Start Bluetooth HID emulation so the ESP32 acts like a keyboard.
    bleKeyboard.begin();
    Serial.println("BLE keyboard started. Connect to it from your computer or phone.");
}

// Called continuously after startup. This loop reads the keypad, debounces presses,
// and sends the appropriate keyboard output to the host device.
void loop() {
    // Track the most recent candidate key and the last key that was reported.
    static char candidateKey = '\0';
    static char reportedKey = '\0';
    static unsigned long candidateSince = 0;

    // Read the keypad state once per loop iteration.
    const char currentKey = scanKeypad();

    // If the detected key changed, start a new debounce timer for that candidate.
    if (currentKey != candidateKey) {
        candidateKey = currentKey;
        candidateSince = millis();
    }

    // Only act on a key after it stays stable for the debounce period.
    if (millis() - candidateSince >= DEBOUNCE_MS) {
        // Send a key event only once when a stable press is first detected.
        if (candidateKey != '\0' && candidateKey != reportedKey) {
            Serial.print("Key pressed: ");
            Serial.println(candidateKey);

            // Map the assigned keypad buttons to either special arrow keys or normal text characters.
            switch(candidateKey) {
                case 'A':
                    bleKeyboard.write(KEY_UP_ARROW);
                    Serial.println("Sent UP_ARROW");
                    break;
                case 'B':
                    bleKeyboard.write(KEY_DOWN_ARROW);
                    Serial.println("Sent DOWN_ARROW");
                    break;
                case 'C':
                    bleKeyboard.write(KEY_LEFT_ARROW);
                    Serial.println("Sent LEFT_ARROW");
                    break;
                case 'D':
                    bleKeyboard.write(KEY_RIGHT_ARROW);
                    Serial.println("Sent RIGHT_ARROW");
                    break;
                default:
                    // All remaining keys are sent as regular printable characters.
                    bleKeyboard.print(candidateKey);
                    Serial.print("Sent character: ");
                    Serial.println(candidateKey);
                    break;
            }

            reportedKey = candidateKey;
        }

        // Once the key is released, clear the reported state so the next press can be sent again.
        if (candidateKey == '\0') {
            reportedKey = '\0';
        }
    }

    // Keep the loop responsive and avoid starving other work.
    delay(2);
}
