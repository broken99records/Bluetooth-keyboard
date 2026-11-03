#include <Arduino.h>

// 4x4 membrane keypad layout:
//   1  2  3  A
//   4  5  6  B
//   7  8  9  C
//   *  0  #  D
//
// Connect the keypad's 8-pin ribbon in this order:
//   R1, R2, R3, R4, C1, C2, C3, C4
//
// Change these GPIO numbers if your wiring is different.
constexpr uint8_t ROW_PINS[4] = {13, 12, 14, 27};
constexpr uint8_t COL_PINS[4] = {26, 25, 33, 32};

constexpr char KEYMAP[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

constexpr unsigned long DEBOUNCE_MS = 35;

char scanKeypad() {
    // Rows idle HIGH. Pull one row LOW at a time and look for a LOW column.
    for (uint8_t row = 0; row < 4; ++row) {
        for (uint8_t r = 0; r < 4; ++r) {
            digitalWrite(ROW_PINS[r], HIGH);
        }

        digitalWrite(ROW_PINS[row], LOW);
        delayMicroseconds(5);

        for (uint8_t col = 0; col < 4; ++col) {
            if (digitalRead(COL_PINS[col]) == LOW) {
                return KEYMAP[row][col];
            }
        }
    }

    return '\0'; // No key pressed
}

void setup() {
    Serial.begin(115200);
    delay(200);

    for (uint8_t row = 0; row < 4; ++row) {
        pinMode(ROW_PINS[row], OUTPUT);
        digitalWrite(ROW_PINS[row], HIGH);
    }

    for (uint8_t col = 0; col < 4; ++col) {
        pinMode(COL_PINS[col], INPUT_PULLUP);
    }

    Serial.println();
    Serial.println("ESP32 4x4 keypad ready.");
    Serial.println("Press a key; its character will appear below.");
}

void loop() {
    static char candidateKey = '\0';
    static char reportedKey = '\0';
    static unsigned long candidateSince = 0;

    const char currentKey = scanKeypad();

    if (currentKey != candidateKey) {
        candidateKey = currentKey;
        candidateSince = millis();
    }

    if (millis() - candidateSince >= DEBOUNCE_MS) {
        // Print once when a key becomes stably pressed.
        if (candidateKey != '\0' && candidateKey != reportedKey) {
            Serial.print("Key pressed: ");
            Serial.println(candidateKey);
            reportedKey = candidateKey;
        }

        // Re-arm after all keys are released.
        if (candidateKey == '\0') {
            reportedKey = '\0';
        }
    }

    delay(2);
}
