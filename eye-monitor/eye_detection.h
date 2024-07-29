// Standard library Header file
#include <unistd.h>
#include <iostream>

// Header file for Camera interfacing
#include "libcam2opencv.h"
#include <libcamera/libcamera.h>

// Header file for OpenCV
#include <opencv2/opencv.hpp>

class EyeDetection {
public:

    // Method to load cascade classifiers
    void loadCascades() {
        if (!face_cascade.load("haarcascade_frontalface_alt.xml") || 
            !eye_cascade.load("haarcascade_eye.xml")) {
            std::cerr << "Error loading cascade classifiers!" << std::endl;
            throw std::runtime_error("Error loading cascade classifiers");
        }
    }

    // Method that runs whenever a frame is received
    bool Frame(cv::Mat &frame, const libcamera::ControlList &metadata, int frameCount) {
        if (frameCount == 0) {
            loadCascades();
        }

	// Convert image to grayscale
        cv::Mat gray_image;
        cv::cvtColor(frame, gray_image, cv::COLOR_BGR2GRAY);

        // Detect faces in the grayscale image	
        std::vector<cv::Rect> faces;
        face_cascade.detectMultiScale(gray_image, faces);

        // check if eyes are detected in face
        bool eyes_detected = detectEyes(frame, gray_image, faces);

	return eyes_detected;
	
        
    }

private:
    cv::CascadeClassifier face_cascade, eye_cascade;
    int frameCount;

    // Method to detect eyes within faces
    bool detectEyes(cv::Mat &frame, const cv::Mat &gray_image, const std::vector<cv::Rect> &faces) {
        bool eyes_detected = false;

        for (const auto& face : faces) {
            cv::Mat face_roi = gray_image(face);
            std::vector<cv::Rect> eyes;
            eye_cascade.detectMultiScale(face_roi, eyes);

            if (!eyes.empty()) {
                eyes_detected = true; // Set flag to true if eyes were detected for at least one face
            }
        }

        return eyes_detected;
    }
};

// Function to run eye detection in separate thread
bool runFrameInThread(EyeDetection& eyeDetection, cv::Mat& frame, const libcamera::ControlList& metadata, int frameCount) {
    // Create a promise and future to get the result from the thread
    std::promise<bool> promise;
    std::future<bool> future = promise.get_future();

    // Create a thread to run the Frame method
    std::thread frameThread([&eyeDetection, &frame, &metadata, frameCount, &promise]() {
        bool result = eyeDetection.Frame(frame, metadata, frameCount);
        promise.set_value(result);
    });

    // Join the thread back to the main thread
    frameThread.join();

    // Get the result from the future
    return future.get();
}

