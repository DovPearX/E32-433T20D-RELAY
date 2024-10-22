## This project demonstrates how to communicate wirelessly using the E32-TTL-100 LoRa module. The project includes two main components:

1. **Relay control** based on button input and RF communication.
2. **Configuration and initialization** of the LoRa module for wireless transmission.

## Features

- **Relay Control**: Trigger the relay using a button or receive commands wirelessly over the RF link.
- **LoRa Communication**: Configure and utilize the E32-TTL-100 module to send/receive commands between nodes.
- **Timeout**: The relay turns off after a predefined period of inactivity (3 seconds).
- **ESP32-C3 Compatible**: This project is configured for the ESP32-C3 platform using PlatformIO in Visual Studio Code.

## Components

- ESP32-C3 development board
- E32-TTL-100 LoRa module
- Relay module
- Button
- LED (optional)
- Power supply

## Pin Configuration

- **Button**: Connected to GPIO `10`
- **LED Indicator**: Connected to GPIO `8`
- **Relay**: Connected to GPIO `3`
- **LoRa Module**:
  - **RX**: GPIO `20`
  - **TX**: GPIO `21`
  - **AUX**: GPIO `7`
  - **M0**: GPIO `5`
  - **M1**: GPIO `6`

## How It Works

1. **Button Control**:  
   Pressing the button turns the relay on and sends a message (`"RELAY_ON"`) over the RF link using the E32-TTL-100 LoRa module.

2. **LoRa Communication**:  
   The ESP32-C3 communicates with other nodes via LoRa, receiving commands like `"RELAY_ON"` and `"RELAY_OFF"`, which control the relay.

3. **Timeout**:  
   The relay automatically turns off after a predefined timeout period (3 seconds).

## Task Structure (FreeRTOS)

- **Button Task**: Monitors button presses and triggers relay control.
- **Receive Task**: Listens for incoming RF messages and triggers corresponding actions.
- **Timeout Task**: Ensures the relay turns off after inactivity.

#### Key Functions

- `relay_on()`: Turns the relay on.
- `relay_off()`: Turns the relay off.
- `buttonTask()`: Monitors the button state and sends relay control messages.
- `receiveTask()`: Listens for messages via the LoRa module and triggers actions.
- `timeoutTask()`: Checks for relay inactivity and turns it off if necessary.

#### Key Configuration Details

- **Pins for LoRa Module**:
  - RX: GPIO `20`
  - TX: GPIO `21`
  - AUX: GPIO `7`
  - M0: GPIO `5`
  - M1: GPIO `6`

- **LoRa Configuration Parameters**:
  - Address: `0x0100`
  - Channel: `0x19` (default channel for communication)
  - Air Data Rate: `0.03 kbps`
  - UART Baud Rate: `9600 bps`
  - Parity: `8N1`
  - Transmission Power: `20 dBm`
  - Wakeup Time: `250 ms`

# License
This project is licensed under the WTFPL (Do What The F*ck You Want To Public License).
