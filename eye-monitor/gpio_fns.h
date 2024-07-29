// Standard library Header files
#include <unistd.h>
#include <iostream>

// Header file for GPIO access
#include <pigpio.h>

// Defining constants
#define buzzer 16 // buzzer attached to GPIO 16 (Pin 36)
#define led_eye_detect 20 // LED attached to GPIO 20 (Pin38)
#define relay 21 // relay attached to GPIO 21 (Pin40) (optional - to trigger eCall system)
#define ON 1 
#define OFF 0

// Class defined to initialise, de-initialise and operate GPIO pins
class GPIOctrl {
public:
    GPIOctrl();
    ~GPIOctrl();
    void initializeGPIO();
    void cleanupGPIO();
    
};    

GPIOctrl::GPIOctrl() {
}

GPIOctrl::~GPIOctrl() {
    cleanupGPIO();
}

// Function to initialize GPIO
void GPIOctrl::initializeGPIO() {
    if (gpioInitialise() < 0) {
        std::cerr << "Error initializing GPIO library" << std::endl;
        return;
    }

    // Set GPIO pin modes
    gpioSetMode(buzzer, PI_OUTPUT);
    gpioSetMode(led_eye_detect, PI_OUTPUT);
    gpioSetMode(relay, PI_OUTPUT);

    return;
}

// Function to clean up GPIO
void GPIOctrl::cleanupGPIO() {
    // Set GPIO pins back to input mode
    gpioSetMode(buzzer, PI_INPUT);
    gpioSetMode(led_eye_detect, PI_INPUT);
    gpioSetMode(relay, PI_INPUT);

    // Terminate pigpio library
    gpioTerminate();
}


