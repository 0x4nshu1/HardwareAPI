#include "hardware_api.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        // Serial communication example
        HardwareAPI::SerialPort serial("/dev/ttyUSB0", B9600);
        if (serial.open()) {
            std::cout << "Serial port opened successfully" << std::endl;
            uint8_t data[] = "Hello, Serial!";
            serial.write(data, sizeof(data));
            serial.close();
        } else {
            std::cerr << "Failed to open serial port" << std::endl;
        }

        // Sensor reading example
        HardwareAPI::TemperatureSensor tempSensor("/sys/bus/w1/devices/28-00000XXXXXXX/w1_slave");
        double temperature = tempSensor.read();
        std::cout << "Temperature: " << temperature << " °C" << std::endl;

        HardwareAPI::PressureSensor pressureSensor("/dev/pressure_sensor");
        double pressure = pressureSensor.read();
        std::cout << "Pressure: " << pressure << " hPa" << std::endl;

        // LED control example
        HardwareAPI::LED led(18); // GPIO pin 18
        led.turnOn();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        led.turnOff();

        // Motor control example
        HardwareAPI::Motor motor(12, 16, 20); // PWM pin 12, direction pins 16 and 20
        motor.setSpeed(50); // 50% speed forward
        std::this_thread::sleep_for(std::chrono::seconds(2));
        motor.stop();

        // GPIO example
        HardwareAPI::GPIO gpio(23, HardwareAPI::GPIO::Direction::INPUT);
        HardwareAPI::GPIO::Value value = gpio.getValue();
        std::cout << "GPIO 23 value: " << (value == HardwareAPI::GPIO::Value::HIGH ? "HIGH" : "LOW") << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}