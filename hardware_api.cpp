#include "hardware_api.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>
#include <fstream>
#include <stdexcept>
#include <cstring>

namespace HardwareAPI {

// SerialPort implementation
SerialPort::SerialPort(const std::string& port, int baudRate)
    : portName(port), baudRate(baudRate), fd(-1) {}

SerialPort::~SerialPort() {
    close();
}

bool SerialPort::open() {
    fd = ::open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) return false;

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, baudRate);
    cfsetospeed(&options, baudRate);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);

    return true;
}

void SerialPort::close() {
    if (fd != -1) {
        ::close(fd);
        fd = -1;
    }
}

bool SerialPort::isOpen() const {
    return fd != -1;
}

int SerialPort::read(uint8_t* buffer, size_t size) {
    return ::read(fd, buffer, size);
}

int SerialPort::write(const uint8_t* data, size_t size) {
    return ::write(fd, data, size);
}

// TemperatureSensor implementation
TemperatureSensor::TemperatureSensor(const std::string& device) : devicePath(device) {}

double TemperatureSensor::read() {
    std::ifstream file(devicePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open temperature sensor device");
    }
    double temperature;
    file >> temperature;
    return temperature;
}

// PressureSensor implementation
PressureSensor::PressureSensor(const std::string& device) : devicePath(device) {}

double PressureSensor::read() {
    std::ifstream file(devicePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open pressure sensor device");
    }
    double pressure;
    file >> pressure;
    return pressure;
}

// LED implementation
LED::LED(int pin) : pin(pin), state(false) {
    // Initialize GPIO for LED
    GPIO gpio(pin, GPIO::Direction::OUTPUT);
    gpio.setValue(GPIO::Value::LOW);
}

void LED::turnOn() {
    GPIO gpio(pin, GPIO::Direction::OUTPUT);
    gpio.setValue(GPIO::Value::HIGH);
    state = true;
}

void LED::turnOff() {
    GPIO gpio(pin, GPIO::Direction::OUTPUT);
    gpio.setValue(GPIO::Value::LOW);
    state = false;
}

void LED::toggle() {
    if (state) {
        turnOff();
    } else {
        turnOn();
    }
}

bool LED::getState() const {
    return state;
}

// Motor implementation
Motor::Motor(int pwmPin, int dirPin1, int dirPin2)
    : pwmPin(pwmPin), dirPin1(dirPin1), dirPin2(dirPin2) {
    // Initialize GPIO for motor control
    GPIO pwm(pwmPin, GPIO::Direction::OUTPUT);
    GPIO dir1(dirPin1, GPIO::Direction::OUTPUT);
    GPIO dir2(dirPin2, GPIO::Direction::OUTPUT);
    stop();
}

void Motor::setSpeed(int speed) {
    GPIO pwm(pwmPin, GPIO::Direction::OUTPUT);
    GPIO dir1(dirPin1, GPIO::Direction::OUTPUT);
    GPIO dir2(dirPin2, GPIO::Direction::OUTPUT);

    if (speed > 0) {
        dir1.setValue(GPIO::Value::HIGH);
        dir2.setValue(GPIO::Value::LOW);
    } else if (speed < 0) {
        dir1.setValue(GPIO::Value::LOW);
        dir2.setValue(GPIO::Value::HIGH);
    } else {
        dir1.setValue(GPIO::Value::LOW);
        dir2.setValue(GPIO::Value::LOW);
    }

    // Set PWM duty cycle (simplified, assumes PWM is already set up)
    int dutyCycle = std::abs(speed);
    (void)dutyCycle; // Avoid unused variable warning
}

void Motor::stop() {
    setSpeed(0);
}

// GPIO implementation
GPIO::GPIO(int pin, Direction direction) : pin(pin), direction(direction) {
    // Open GPIO device
    int fd = open("/dev/gpiochip0", O_RDWR);
    if (fd < 0) {
        throw std::runtime_error("Failed to open GPIO device");
    }

    // Request GPIO line
    struct gpiohandle_request req;
    req.lineoffsets[0] = pin;
    req.flags = (direction == Direction::OUTPUT) ? GPIOHANDLE_REQUEST_OUTPUT : GPIOHANDLE_REQUEST_INPUT;
    req.lines = 1;
    if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req) < 0) {
        close(fd);
        throw std::runtime_error("Failed to request GPIO line");
    }

    close(fd);
}

void GPIO::setDirection(Direction newDirection) {
    direction = newDirection;
}

void GPIO::setValue(Value value) {
    if (direction != Direction::OUTPUT) {
        throw std::runtime_error("Cannot set value on input GPIO");
    }

    // Set GPIO value
    struct gpiohandle_data data;
    data.values[0] = (value == Value::HIGH) ? 1 : 0;
    if (ioctl(pin, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        throw std::runtime_error("Failed to set GPIO value");
    }
}

GPIO::Value GPIO::getValue() const {
    struct gpiohandle_data data;
    if (ioctl(pin, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
        throw std::runtime_error("Failed to get GPIO value");
    }
    return (data.values[0] == 1) ? Value::HIGH : Value::LOW;
}

} // namespace HardwareAPI