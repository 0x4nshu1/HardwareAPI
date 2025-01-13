# Hardware Interaction API

## Overview

The Hardware Interaction API is a C++ library designed to simplify interactions with various hardware components. It provides a unified interface for serial communication, sensor data reading, basic hardware control, and GPIO interfacing, making it ideal for projects involving microcontrollers like Raspberry Pi.

## Features

- **Serial Communication**: Read from and write to COM ports.
- **Sensor Data Reading**: Interface with temperature and pressure sensors.
- **Basic Hardware Control**: Manage LEDs and control motors.
- **GPIO Interface**: Interact with General Purpose Input/Output pins.

## Prerequisites

- C++11 compatible compiler
- Linux-based system (the current implementation uses Linux-specific headers)
- Access to hardware devices (serial ports, sensors, GPIO pins)

## Installation

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/hardware-interaction-api.git
   cd hardware-interaction-api
   ```

2. Compile the library:
   ```
   g++ -c -std=c++11 hardware_api.cpp
   ```

3. Link the library with your project:
   ```
   g++ -std=c++11 your_project.cpp hardware_api.o -o your_project
   ```

## Usage

Here are some basic examples of how to use the Hardware Interaction API:

### Serial Communication

```cpp
HardwareAPI::SerialPort serial("/dev/ttyUSB0", B9600);
if (serial.open()) {
    uint8_t data[] = "Hello, Serial!";
    serial.write(data, sizeof(data));
    serial.close();
}
```

### Sensor Reading

```cpp
HardwareAPI::TemperatureSensor tempSensor("/sys/bus/w1/devices/28-00000XXXXXXX/w1_slave");
double temperature = tempSensor.read();
std::cout << "Temperature: " << temperature << " °C" << std::endl;
```

### LED Control

```cpp
HardwareAPI::LED led(18); // GPIO pin 18
led.turnOn();
std::this_thread::sleep_for(std::chrono::seconds(1));
led.turnOff();
```

### Motor Control

```cpp
HardwareAPI::Motor motor(12, 16, 20); // PWM pin 12, direction pins 16 and 20
motor.setSpeed(50); // 50% speed forward
std::this_thread::sleep_for(std::chrono::seconds(2));
motor.stop();
```

### GPIO Operations

```cpp
HardwareAPI::GPIO gpio(23, HardwareAPI::GPIO::Direction::INPUT);
HardwareAPI::GPIO::Value value = gpio.getValue();
std::cout << "GPIO 23 value: " << (value == HardwareAPI::GPIO::Value::HIGH ? "HIGH" : "LOW") << std::endl;
```

## Contributing

Contributions to the Hardware Interaction API are welcome! Please feel free to submit pull requests, create issues or spread the word.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Disclaimer

This API is provided as-is, and may require adjustments depending on your specific hardware and operating system. Always ensure proper safety measures when working with hardware components.
