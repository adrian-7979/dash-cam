// Standard library Header file
#include <unistd.h>
#include <iostream>

// Header file for GPIO access, user functions and pin declarations
#include <pigpio.h>
#include "gpio_fns.h"

// Header file for threads
#include <pthread.h>

// Header files for playing sounds (.wav files) and playSound() function
#include <alsa/asoundlib.h>
#include "sound.h"

// Header file for Camera interfacing
#include "libcam2opencv.h"
#include <libcamera/libcamera.h>

// Header file for OpenCV
#include <opencv2/opencv.hpp>

// Definitions:
// Number of frames(with eyes not detected) after which buzzer rings
#define MIN_FRAMES_B 4
// Number of frames(with eyes not detected) after which relay switches ON 
#define MIN_FRAMES_R 20

/**********************************************************************/

// Declaring the object as a global variable for use in multiple functions
AudioPlayer player();
    

// Callback function
struct MyCallback : Libcam2OpenCV::Callback {
   int frameCount=0;
   int frameEyeShut=0;
   // Load the pre-trained face and eye cascade classifiers
    cv::CascadeClassifier face_cascade, eye_cascade;

   // initialise sound thread
   pthread_t soundThread;
   
   // function that runs whenever frame is received
    virtual void hasFrame(const cv::Mat &frame, const libcamera::ControlList &metadata) {
	
     // initialise GPIO 
    initializeGPIO();
    
    // load cascades at the 0th frame 
     if(frameCount ==0){
        if(!face_cascade.load("haarcascade_frontalface_alt.xml") || !eye_cascade.load("haarcascade_eye.xml")) {
         std::cerr << "Error loading cascade classifiers!" << std::endl;
         throw std::runtime_error("Error loading cascade classifiers");
    } }


    // Convert image to grayscale
    cv::Mat gray_image;
    cv::cvtColor(frame, gray_image, cv::COLOR_BGR2GRAY);

    // Detect faces in the grayscale image
    std::vector<cv::Rect> faces;
    face_cascade.detectMultiScale(gray_image, faces);

    // Flag to track if eyes were detected
    bool eyes_detected = false;

    // For each face, detect eyes
    for (const auto& face : faces) {

        // Define region of interest (ROI) for eyes within the face
        cv::Mat face_roi = gray_image(face);
        std::vector<cv::Rect> eyes;
        eye_cascade.detectMultiScale(face_roi, eyes);

   // Testing: Draw rectangles and circles on required ROI (for debugging, uncomment below lines)
   //cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2); // Draw rectangle around face
   //for (const auto& eye : eyes) {
   //     cv::Point eye_center(face.x + eye.x + eye.width/2, face.y + eye.y + eye.height/2);
   //     int radius = cvRound((eye.width + eye.height) * 0.25);
   //     cv::circle(frame, eye_center, radius, cv::Scalar(0, 255, 0), 2); // Draw circle around eye
   //}
        // Display the resulting image (for debugging, uncomment below lines)
   //	 cv::imshow("Faces and Eyes Detection", frame);
   //  	 cv::waitKey(0);
   //  	 cv::destroyAllWindows();

        // Check if eyes were detected within the face
        if (!eyes.empty()) {
            eyes_detected = true; // Set flag to true if eyes were detected for at least one face
        }
    }

    // Display FrameCount
    std::cout << frameCount << std::endl;
    
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
        if((frameEyeShut%8)==0 || (frameEyeShut==MIN_FRAMES_B) )
        pthread_create(&soundThread, NULL, AudioPlayer::threadFunc, NULL); 
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
    initializeGPIO();
    
    
    std::cout << "Press any key to stop" << std::endl;
    
    // create an instance of the callback
    MyCallback myCallback;

    // register the callback
    camera.registerCallback(&myCallback);

    // create an instance of the settings
    Libcam2OpenCVSettings settings;

    // set the framerate (default is variable framerate)
    settings.framerate = 10;

    // start the camera with these settings
    camera.start(settings);

    // do nothing till the user presses any key
    getchar();

    // stop the camera
    camera.stop();
    
    // set the GPIO pins back to input mode
    cleanupGPIO();

    // that's it!
    printf("\n");
}
