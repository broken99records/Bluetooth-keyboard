# Bluetooth Numpad and Arrow Keys

This project turns an ESP32 Dev Board into a Bluetooth HID keyboard controlled by a 4x4 matrix keypad that combines a numpad with arrow-key navigation.

The combination is useful because it gives you both number entry and directional control in one compact input device. Instead of needing a separate keypad and a mouse or keyboard arrows, this design puts both into a small, tactile layout.

The firmware reads the keypad, debounces button presses, and sends either:
- standard printable characters for keys like `1`, `2`, `3`, `*`, `0`, and `#`
- arrow keys for the `A`, `B`, `C`, and `D` keys

This makes the board especially handy for quick navigation, media control, spreadsheet input, kiosk-style control panels, or any setup where compact numeric + directional control is useful.

It is designed for use with PlatformIO and the ESP32 Arduino framework.

## Why this combination is useful

The numpad and arrow-key pairing is a very practical layout because it combines two common input needs in one place:

- use the number pad for typing digits, codes, values, and shortcuts
- use the arrow keys for moving through menus, lists, documents, and interfaces
- keep the device compact while improving speed and control
- avoid carrying or wiring a separate navigation device

## Features

- ESP32-based Bluetooth keyboard emulation
- Numpad and arrow-key layout
- 4x4 membrane keypad input matrix
- Debounced key detection to prevent repeated presses
- Serial debug output for keypad activity
- Arrow key support mapped to the keypad's A/B/C/D buttons

## Hardware layout

The keypad is wired as a 4x4 matrix using 8 connections:
- rows: R1, R2, R3, R4
- columns: C1, C2, C3, C4

Key layout used in the firmware:

```text
1  2  3  A
4  5  6  B
7  8  9  C
*  0  #  D
```

The code currently maps these GPIO pins on the ESP32:

```cpp
constexpr uint8_t ROW_PINS[4] = {13, 12, 14, 27};
constexpr uint8_t COL_PINS[4] = {26, 25, 33, 32};
```

If your physical wiring differs, update those values in `src/main.cpp`.

## Key mapping

| Keypad key | Behavior |
| --- | --- |
| `A` | Sends `KEY_UP_ARROW` |
| `B` | Sends `KEY_DOWN_ARROW` |
| `C` | Sends `KEY_LEFT_ARROW` |
| `D` | Sends `KEY_RIGHT_ARROW` |
| `1-9`, `*`, `0`, `#` | Sends the matching character |

## Project structure

```text
.
├── include/
├── lib/
├── src/
│   └── main.cpp
├── test/
├── platformio.ini
├── .gitignore
└── README.md
```

## Requirements

- PlatformIO
- ESP32 development board
- 4x4 keypad matrix
- USB cable for flashing the ESP32

## Setup

1. Install PlatformIO VsCode extension and open this project.
2. Connect the ESP32 to your computer.
3. Build and upload the firmware:

```bash
pio run -t upload
```

4. Open the serial monitor to watch debugging output:

```bash
pio device monitor
```

5. Pair the ESP32 as a Bluetooth keyboard from your host device.

## Usage

After the board boots:
- the serial monitor prints a startup banner
- the ESP32 initializes Bluetooth keyboard mode
- pressing a keypad button sends the mapped keyboard input to the connected device

## Notes

- The key detection routine uses a short debounce window to avoid registering repeated events from a single press.
- `A` through `D` are treated as navigation keys instead of regular characters.
- If the keyboard does not appear or input is unreliable, verify your keypad wiring and GPIO assignments.

## License

This project is provided as-is for educational and personal hardware experimentation.
