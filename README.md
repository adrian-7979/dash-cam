# Driver Monitor System
A camera system that detects if the driver is asleep/unconscious and rings an alarm.
It detects the eyes of the driver, and if the eyes are not detected for some amount of consecutive
frames, the alarm sound is played using the AUX port of the Raspberry Pi.


## Acknowledgements
We would like to thank Bernd Porr for his contribution to the "libcamera2opencv".
The original code can be found at github.com/berndporr/libcamera2opencv 

We would also like to thank Nash for his contribution to the eye tracking code.
The original code can be found at github.com/bsdnoobz/opencv-code/blob/master/eye-tracking.cpp 


## Prerequisites

```
apt install libopencv-dev libcamera-dev
```

This program also uses ALSA to play the alarm sound.
```
sudo apt-get install libasound2-dev
```
## Compilation and installation

```
cmake .
make
sudo make install
```

## Output File
The output file "eye" is in the subdirectory "eye-monitor", and can be run
in Linux terminal with the command
```
./eye
```
