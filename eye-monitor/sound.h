/**
 * @file AudioPlayer.h
 * @brief Header file containing the AudioPlayer class definition and implementation for playing .wav files using ALSA.
 */

#include <iostream>
#include <alsa/asoundlib.h>

/**
 * @class AudioPlayer
 * @brief A class to play a sound file using ALSA.
 *
 * This class provides methods to open a PCM device, configure it, and play a .wav sound file.
 */

class AudioPlayer {
public:
    /**
     * @brief Constructor for the AudioPlayer class.
     *
     * Initializes the AudioPlayer object.
     */
    AudioPlayer();

   /**
     * @brief Destructor for the AudioPlayer class.
     *
     * Cleans up resources used by the AudioPlayer.
     */
    ~AudioPlayer();

     /**
     * @brief Plays the sound file.
     *
     * This method orchestrates the opening of the PCM device, setting parameters, reading the sound file, and playing it.
     */
    void playSound();

    /**
     * @brief Static member function is used as a thread function.
     *
     * @param arg A pointer to an AudioPlayer object.
     * @return A nullptr.
     *
     * This function calls the playSound() method of the AudioPlayer object.
     */
    static void* threadFunc(void* arg); // Static member function

private:
    /**
     * @brief Opens the PCM device for playback.
     *
     * Checks for and opens the PCM device.
     */

    void openPCMDevice();        
    /**
     * @brief Configures the PCM device for playing sound.
     *
     * Sets the parameters for the PCM device.
     */
    void setPCMParams();         

    /**
     * @brief Opens the sound file for reading.
     *
     * Opens the .wav file specified by filePath.
     */
    void openSoundFile();        
    
    /**
     * @brief Reads and plays the sound file.
     *
     * Reads data from the .wav file and writes it to the PCM device for playback.
     */
    void readAndPlaySound();
    
    /**
     * @brief Cleans up resources.
     *
     * Resets configuration and closes the PCM device and sound file.
     */
    void cleanup();             

    const char* filePath = "sound.wav"; //< Path to the .wav sound file.
    snd_pcm_t *pcm_handle; ///< Handle for the PCM device.
    FILE *wav_file; ///< File pointer for the .wav sound file.
}; 

//AudioPlayer::AudioPlayer(const char* filePath) : filePath(filePath), pcm_handle(nullptr), wav_file(nullptr) {

/**
 * @brief Constructor and Destructor implementation.
 */
AudioPlayer::AudioPlayer() {
}

AudioPlayer::~AudioPlayer() {
    cleanup();
}

/**
 * @brief Static member function implementation.
 *
 * @param arg A pointer to an AudioPlayer object.
 * @return A nullptr.
 */
void* AudioPlayer::threadFunc(void* arg) {
    AudioPlayer* player = static_cast<AudioPlayer*>(arg);
    player->playSound();
    return nullptr;
}

/**
 * @brief Public function which plays the sound.
 */
void AudioPlayer::playSound() {
    openPCMDevice();
    setPCMParams();
    openSoundFile();
    readAndPlaySound();
    cleanup();
}

/**
 * @brief Opens the PCM device for playback.
 */
void AudioPlayer::openPCMDevice() {
    if (snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        std::cerr << "Error opening PCM device" << std::endl;
        pthread_exit(NULL);
    }
}

/**
 * @brief Sets parameters for the PCM device.
 */
void AudioPlayer::setPCMParams() {
    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE); // Signed 16-bit little-endian
    snd_pcm_hw_params_set_channels(pcm_handle, params, 2); // Stereo
    unsigned int sample_rate = 44100;
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &sample_rate, 0);
    snd_pcm_hw_params(pcm_handle, params);
    snd_pcm_prepare(pcm_handle);
}

/**
 * @brief Opens the sound file for reading.
 */
void AudioPlayer::openSoundFile() {
    wav_file = fopen(filePath, "r");
    if (!wav_file) {
        std::cerr << "Error opening sound file: " << filePath << std::endl;
        snd_pcm_close(pcm_handle);
        pthread_exit(NULL);
    }
}

/**
 * @brief Reads and plays the sound file.
 */
void AudioPlayer::readAndPlaySound() {
    const int buffer_size = 4096;
    char buffer[buffer_size];
    size_t read_size;

    while ((read_size = fread(buffer, 1, buffer_size, wav_file)) > 0) {
        if (snd_pcm_writei(pcm_handle, buffer, read_size / 4) < 0) { // Dividing by 4 because sample size is 16-bit = 2 bytes
            std::cerr << "Error playing audio" << std::endl;
            break;
        }
    }
}

/**
 * @brief Close PCM handle and sound file and cleans up.
 */
void AudioPlayer::cleanup() {
    if (wav_file) {
        fclose(wav_file);
        wav_file = nullptr;
    }
    if (pcm_handle) {
        snd_pcm_drain(pcm_handle);
        snd_pcm_close(pcm_handle);
        pcm_handle = nullptr;
    }
    pthread_exit(NULL);
}



