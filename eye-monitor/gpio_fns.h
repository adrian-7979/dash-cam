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

/**
 * @class GPIOctrl
 * @brief A class defined to initialize, de-initialize, and operate GPIO pins.
 *
 * This class provides methods to initialize the GPIO pins, set their modes,
 * and clean up the GPIO settings when done.
 */

class GPIOctrl {
public:
    /**
     * @brief Constructor for GPIOctrl class.
     *
     * Initializes the GPIO control object.
     */

    GPIOctrl();
    
    /**
     * @brief Destructor for GPIOctrl class.
     *
     * Cleans up GPIO settings upon object destruction.
     */

    ~GPIOctrl();
    /**
     * @brief Initializes the GPIO pins.
     * Sets up the GPIO library and configures the pin modes.
     *
     * @brief Cleans up the GPIO pins.
     * Resets the GPIO pins to input mode and terminates the GPIO library.
     */
    void initializeGPIO();

    void cleanupGPIO();
    
};    

/**
 * @brief Constructor and destructor implementation.
 */
GPIOctrl::GPIOctrl() {
}

GPIOctrl::~GPIOctrl() {
    cleanupGPIO();
}

/**
 * @brief Function to Initialize the GPIO pins.
 *
 * This function initializes the pigpio library and sets the mode for each GPIO pin.
 * If the library initialization fails, an error message is printed.
 */
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

/**
 * @brief Cleans up the GPIO pins.
 *
 * This function resets the GPIO pins to input mode and terminates the pigpio library.
 */
    
void GPIOctrl::cleanupGPIO() {
    // Set GPIO pins back to input mode
    gpioSetMode(buzzer, PI_INPUT);
    gpioSetMode(led_eye_detect, PI_INPUT);
    gpioSetMode(relay, PI_INPUT);

    // Terminate pigpio library
    gpioTerminate();
}


