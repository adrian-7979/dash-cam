/* Includes playSound function that plays *.wav files  */

// standard header file
#include <iostream>

// Header file for playing sounds (.wav files)
#include <alsa/asoundlib.h>

// Function to play a sound file using ALSA
void playSound() {
    // Open PCM device for playback
    snd_pcm_t *pcm_handle;
    if (snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        std::cerr << "Error opening PCM device" << std::endl;
        //return;
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
    const char* filePath = "sound.wav";
    FILE *wav_file = fopen(filePath, "r");
    if (!wav_file) {
        std::cerr << "Error opening sound file: " << filePath << std::endl;
        snd_pcm_close(pcm_handle);
        pthread_exit(NULL);    //return;
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
    pthread_exit(NULL);
}
