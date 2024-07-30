// Standard library Header file
#include <unistd.h>
#include <iostream>

// Header file for Camera interfacing
#include "libcam2opencv.h"
#include <libcamera/libcamera.h>

// Header file for OpenCV
#include <opencv2/opencv.hpp>

/**
* * @class EyeDetection
 * @brief A class for detecting eyes in a camera frame.
 *
 * This class uses OpenCV's Cascade Classifier to detect faces and eyes in each frame captured by the camera.
 */

class EyeDetection {
public:

    /**
     * @brief Loads the cascade classifiers for facial and eye detection.
     *
     * This method loads the Haar cascade classifiers for face and eye detection. 
     * Throws an exception if any of the classifiers fail to load.
     */

    void loadCascades() {
        if (!face_cascade.load("haarcascade_frontalface_alt.xml") || 
            !eye_cascade.load("haarcascade_eye.xml")) {
            std::cerr << "Error loading cascade classifiers!" << std::endl;
            throw std::runtime_error("Error loading cascade classifiers");
        }
    }

/**
     * @brief Frame processing to detect eyes
     *
     * This method converts the frame to grayscale, detects faces, and then checks for eyes within each detected face.
     *
     * @param frame The input image frame in which eyes will be detected.
     * @param frameCount The number of frames being processed so far.
     * @return Returns true if eyes are detected in the frame, false otherwise.
     */

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

    /**
     * @brief Detects eyes within the detected faces.
     *    
     * This method iterates through the detected faces and attempts to detect eyes within each face region.
     *
     * @param frame The input image frame.
     * @param gray_image The grayscale version of the frame.
     * @param faces A list of rectangles representing detected faces.
     * @return Returns true if eyes are detected within at least one face, false otherwise.
     */

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

/**
 * @brief Runs the Frame method for eye detection in separate thread.
 * 
 * This function creates a thread to execute the Frame method and returns the result.
 *
 * @param eyeDetection Instance of EyeDetection used to process the frame.
 * @param frame The input image frame to be processed.
 * @param frameCount The number of frames processed so far.
 * @return Returns true if eyes are detected in the frame, false otherwise.
 */

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

