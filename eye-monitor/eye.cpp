// Standard library Header file
#include <unistd.h>
#include <iostream>

// Header file for GPIO access, user functions and pin declarations
#include <pigpio.h>
#include "gpio_fns.h"

// Header file for multi-threading, and returning values from threads
#include <thread>
#include <future>

// Header files for playing sounds (.wav files) and playSound() function
#include <alsa/asoundlib.h>
#include "sound.h"

// Header file for Camera interfacing
#include "libcam2opencv.h"
#include <libcamera/libcamera.h>

// Header file for OpenCV
#include <opencv2/opencv.hpp>
#include "eye_detection.h"

// Definitions:
// Number of frames(with eyes not detected) after which buzzer rings
#define MIN_FRAMES_B 4
// Number of frames(with eyes not detected) after which relay switches ON 
#define MIN_FRAMES_R 20

/**********************************************************************/

// Declaring the object as a global variable for use in multiple functions
AudioPlayer player;
GPIOctrl gpioCtrl;     
EyeDetection eyeDetection;

// Callback function
struct MyCallback : Libcam2OpenCV::Callback {
   int frameCount=0; // counter for number of frames
   int frameEyeShut=0; // counter for number of consecutive frames with eyes closed

// run function when frame is received
    virtual void hasFrame(cv::Mat &frame, const libcamera::ControlList &metadata)	
{	
   // Load the pre-trained face and eye cascade classifiers
    cv::CascadeClassifier face_cascade, eye_cascade;
     
    // Scan face and detect if eyes are open
    bool eyes_detected = runFrameInThread(eyeDetection, frame, metadata, frameCount);

    // Display FrameCount
    std::cout << frameCount << std::endl;
 
     // initialise GPIO 
    gpioCtrl.initializeGPIO();
    
    // Display appropriate message based on eyes detection
    if (eyes_detected) {
        std::cout << "Eyes Detected!" << std::endl;
        //reset counter
        frameEyeShut = 0; 
        // Turn ON the LED (indicate eyes are detected)
        gpioWrite(led_eye_detect, ON); 
        // Turn OFF buzzer 
        gpioWrite(buzzer, OFF);
        // Turn OFF relay(eCall system) 
        gpioWrite(relay, OFF);
         
    } 
    else {
        std::cout << "No eyes detected in the image!" << std::endl;
        frameEyeShut++; //increment counter
        // Turn OFF the LED (indicate eyes are NOT detected)
        gpioWrite(led_eye_detect, OFF); 
    }    
    
    // if eyes are closed for short time
    if(frameEyeShut>= MIN_FRAMES_B){
        // Turn ON buzzer 
        gpioWrite(buzzer, ON);
        
        // Play sound file on different thread, fewer times
        if((frameEyeShut%10)==0 || (frameEyeShut==MIN_FRAMES_B) )
            std::thread soundThread(&AudioPlayer::threadFunc, nullptr); 
    }

    // if eyes are closed for long time even after buzzer rings, trigger eCall system
    if(frameEyeShut>=MIN_FRAMES_R){
        // Turn ON relay(eCall system) 
        gpioWrite(relay, ON);
        frameEyeShut=0; //reset counter
    }
    
    std::cout << "Eyes shut for "<< frameEyeShut << " frames" << std::endl; //print counter value

	frameCount++;
 }   
};

/**********************************************************************/

// Main program
int main(int argc, char *argv[]) {
    
    
    
    // create an instance of the camera class
    Libcam2OpenCV camera;
    
    // initialise GPIO 
    gpioCtrl.initializeGPIO();
    
    
    std::cout << "Press any key to stop" << std::endl;
    
    // create an instance of the callback
    MyCallback myCallback;

    // register the callback
    camera.registerCallback(&myCallback);

    // create an instance of the settings
    Libcam2OpenCVSettings settings;

    // set the framerate (default is variable framerate)
    settings.framerate = 30;

    // start the camera with these settings
    camera.start(settings);

    // do nothing till the user presses any key
    getchar();

    // stop the camera
    camera.stop();
    
    // set the GPIO pins back to input mode
    gpioCtrl.cleanupGPIO();

    // that's it!
    printf("\n");
}
