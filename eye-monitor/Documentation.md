# Driver Monitoring System Documentaion

Here are the brief description for the classes its methods, structs, callbacks used in the driver-monitoring system code. It also comprises multi-threading to handle audio playback without blocking the main execution flow.

## List of Classes

### **Eye Detection** 
Class to load Cascade classifiers. Consists of haarcascade codes which is used to facial and eye detection. OpenCV is used to detect eyes in the camera frames.

`Method Frame`             : Converts image to grayscale and further detects the face in image.

`Method detectEyes`        : Checks if the eyes are detected within the face and returns a boolean value(True/Flase) indicating the presence or absence of eyes.

There is an additional function that executes the face and eye detection code in a separate thread. 

`Method runFrameInThread`  : To ensure that the boolean result of the detection process can be accessed and utilized in the main thread,

We use
`std::promise and std::future` : This approach allows for efficient thread communication and synchronization, enabling the main thread to retrieve the result produced by the eyeDetection class seamlessly.

---------------------------------------------------------------------------------------------------------------------------
### **GPIOctrl**

Class manages the GPIO pins on the Raspberry Pi.

`Method initializeGPIO` : Initializes the GPIO pins, configuring them to control various peripherals. Specifically, sets the pins for the buzzer, LED, and relay as output pins.

`Method cleanupGPIO` : Resets the GPIO pins to their default state, ensuring that all the pins are properly released and terminates GPIO access. This is typically called when the application is shutting down to ensure a clean exit.

---------------------------------------------------------------------------------------------------------------------------
### **AudioPlayer**

This class uses **sound.h* file for the alert system in the vehicle speaker. Handles audio playback using **ALSA**. It is connected to the audio port of the Raspberry Pi through an AUX cable.

`Method threadFunc` runs `Method playSound` on a different thread that includes the *.wav flies*.

--------------------------------------------------------------------------------------------------------------------------
### Frames Captured 

`MIN_FRAMES_B`: Number of frames with eyes not detected before the buzzer rings is fixed to be **4 frames**.

`MIN_FRAMES_R`: Number of frames with eyes not detected before the relay switches ON is **20 Frames**.
  
### **Callback Function**

`frameCount`   : Counter function for number of frames recorded.

`frameEyeShut` : Counter function for number of consecutive frames with eyes closed.

`Run function **hasFrame**` : processes each frame received from the camera.

The **Eye Detection** updates frame counters and sets GPIO states based on whether eyes are detected.

The **detectEyes** resets **frameEyeShut**.

        -When `Eyes is detected` it turns ON the LED, and turns OFF the buzzer and relay.
        
        -When `Eyes is Not Detected`: Increments `frameEyeShut`, turns OFF the LED.
        
        -If `frameEyeShut` exceeds `MIN_FRAMES_B`, turns ON the buzzer and plays a sound in a separate thread.
        
        -If `frameEyeShut` exceeds `MIN_FRAMES_R`, turns ON the relay and resets `frameEyeShut`.

Finally, The **GPIOctrl** cleans up the GPIO resetting the GPIO pins used for the buzzer, LED and relay to their default state (input mode).

`gpioTerminate Library` : Calls the gpioTerminate function to shut down the pigpio library, ensuring that resources allocated by the library are properly cleaned up.
