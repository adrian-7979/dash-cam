#include <unistd.h>

// Header file for Camera interfacing
#include "libcam2opencv.h"
#include <libcamera/libcamera.h>

// Header file for OpenCV
#include <opencv2/opencv.hpp>

// Header file for GPIO access
#include <pigpio.h>

// Defining constants
#define buzzer 16 // buzzer attached to GPIO 16 (Pin 36)
#define led_eye_detect 20 // LED attached to GPIO 20 (Pin38)
#define relay 21 // relay attached to GPIO 21 (Pin40) (optional - to trigger eCall system)
#define ON 1 
#define OFF 0

// Header file for playing sounds (.wav files)
#include <alsa/asoundlib.h>
// Function to play a sound file using ALSA
void playSound(const char* filePath) {
    // Open PCM device for playback
    snd_pcm_t *pcm_handle;
    if (snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        std::cerr << "Error opening PCM device" << std::endl;
        return;
    }

    // Set PCM parameters
    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE); // Signed 16-bit little-endian
    snd_pcm_hw_params_set_channels(pcm_handle, params, 2); // Stereo
    unsigned int sample_rate = 44100;
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sample_rate, 0);
    snd_pcm_hw_params(pcm_handle, params);

    // Prepare PCM device
    snd_pcm_prepare(pcm_handle);

    // Open the sound file
    FILE *wav_file = fopen(filePath, "r");
    if (!wav_file) {
        std::cerr << "Error opening sound file: " << filePath << std::endl;
        snd_pcm_close(pcm_handle);
        return;
    }

    // Read and play the sound file
    const int buffer_size = 4096;
    char buffer[buffer_size];
    size_t read_size;

    while ((read_size = fread(buffer, 1, buffer_size, wav_file)) > 0) {
        if (snd_pcm_writei(pcm_handle, buffer, read_size / 4) < 0) { // Dividing by 4 because sample size is 16-bit = 2 bytes
            std::cerr << "Error playing audio" << std::endl;
            break;
        }
    }

    // Close PCM handle and sound file
    fclose(wav_file);
    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
}

// Function to initialize GPIO
void initializeGPIO() {
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
void cleanupGPIO() {
    // Set GPIO pins back to input mode
    gpioSetMode(buzzer, PI_INPUT);
    gpioSetMode(led_eye_detect, PI_INPUT);
    gpioSetMode(relay, PI_INPUT);

    // Terminate pigpio library
    gpioTerminate();
}

// Callback function
struct MyCallback : Libcam2OpenCV::Callback {
   int frameCount=0;
   int frameEyeShut=0;
   // Load the pre-trained face and eye cascade classifiers
    cv::CascadeClassifier face_cascade, eye_cascade;
    
    virtual void hasFrame(const cv::Mat &frame, const libcamera::ControlList &metadata) {
	
     // initialise GPIO 
    initializeGPIO();
     
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

        // Draw rectangles and circles on required ROI (for debugging, uncomment below lines)
   //   cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2); // Draw rectangle around face
   //     for (const auto& eye : eyes) {
   //         cv::Point eye_center(face.x + eye.x + eye.width/2, face.y + eye.y + eye.height/2);
   //         int radius = cvRound((eye.width + eye.height) * 0.25);
   //         cv::circle(frame, eye_center, radius, cv::Scalar(0, 255, 0), 2); // Draw circle around eye
   //     }
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
    if(frameEyeShut>3){
        // Play sound file 
        playSound("sound.wav");
        // Turn ON buzzer 
        gpioWrite(buzzer, ON);
         
    }

    // if eyes are closed for long time even after buzzer rings, trigger eCall system
    if(frameEyeShut>15){
        // Turn OFF relay(eCall system) 
        gpioWrite(relay, OFF);
        frameEyeShut=0; //reset counter
    }
    
    std::cout << "Eyes shut for "<< frameEyeShut << " frames" << std::endl; //print counter value

	frameCount++;
    }
    
};

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
    settings.framerate = 30;

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
