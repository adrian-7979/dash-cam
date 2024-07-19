/* Includes playSound function that plays *.wav files  */

// standard header file
#include <iostream>

// Header file for playing sounds (.wav files)
#include <alsa/asoundlib.h>

// Class defined to play a sound file using ALSA
class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();
    void playSound();
    
    static void* threadFunc(void* arg); // Static member function

private:
    void openPCMDevice();
    void setPCMParams();
    void openSoundFile();
    void readAndPlaySound();
    void cleanup();

    const char* filePath = "sound.wav";
    snd_pcm_t *pcm_handle;
    FILE *wav_file;
}; 

//AudioPlayer::AudioPlayer(const char* filePath) : filePath(filePath), pcm_handle(nullptr), wav_file(nullptr) {
AudioPlayer::AudioPlayer() {
}


AudioPlayer::~AudioPlayer() {
    cleanup();
}

void* AudioPlayer::threadFunc(void* arg) {
    AudioPlayer* player = static_cast<AudioPlayer*>(arg);
    player->playSound();
    return nullptr;
}

// Public function which plays the sound
void AudioPlayer::playSound() {
    openPCMDevice();
    setPCMParams();
    openSoundFile();
    readAndPlaySound();
    cleanup();
}

// Open PCM device for playback
void AudioPlayer::openPCMDevice() {
    if (snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        std::cerr << "Error opening PCM device" << std::endl;
        pthread_exit(NULL);
    }
}

// Set parameters for the PCM device
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

// Open the sound file for reading
void AudioPlayer::openSoundFile() {
    wav_file = fopen(filePath, "r");
    if (!wav_file) {
        std::cerr << "Error opening sound file: " << filePath << std::endl;
        snd_pcm_close(pcm_handle);
        pthread_exit(NULL);
    }
}

// Read and play the sound file
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

// Close PCM handle and sound file
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



