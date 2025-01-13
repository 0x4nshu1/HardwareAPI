#ifndef HARDWARE_API_H
#define HARDWARE_API_H

#include <string>
#include <vector>
#include <cstdint>

namespace HardwareAPI {

    // Serial Communication
    class SerialPort {
    public:
        SerialPort(const std::string& port, int baudRate);
        ~SerialPort();

        bool open();
        void close();
        bool isOpen() const;
        int read(uint8_t* buffer, size_t size);
        int write(const uint8_t* data, size_t size);

    private:
        int fd; // File descriptor for the serial port
        std::string portName;
        int baudRate;
    };

    // Sensor Interface
    class Sensor {
    public:
        virtual ~Sensor() = default;
        virtual double read() = 0;
    };

    class TemperatureSensor : public Sensor {
    public:
        TemperatureSensor(const std::string& device);
        double read() override;

    private:
        std::string devicePath;
    };

    class PressureSensor : public Sensor {
    public:
        PressureSensor(const std::string& device);
        double read() override;

    private:
        std::string devicePath;
    };

    // Hardware Control
    class LED {
    public:
        LED(int pin);
        void turnOn();
        void turnOff();
        void toggle();
        bool getState() const;

    private:
        int pin;
        bool state;
    };

    class Motor {
    public:
        Motor(int pwmPin, int dirPin1, int dirPin2);
        void setSpeed(int speed); // -100 to 100
        void stop();

    private:
        int pwmPin;
        int dirPin1;
        int dirPin2;
    };

    // GPIO Interface
    class GPIO {
    public:
        enum class Direction { INPUT, OUTPUT };
        enum class Value { LOW, HIGH };

        GPIO(int pin, Direction direction);
        void setDirection(Direction direction);
        void setValue(Value value);
        Value getValue() const;

    private:
        int pin;
        Direction direction;
    };
}

#endif // HARDWARE_API_H