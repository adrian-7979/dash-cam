# Driver Monitor System
A camera system that detects if the driver is asleep/unconscious and rings an alarm.
It detects the eyes of the driver, and if the eyes are not detected for some amount of consecutive
frames, the alarm sound is played using the AUX port of the Raspberry Pi.


## Acknowledgements
We would like to thank Bernd Porr for his contribution to the "libcamera2opencv".
The original code can be found at [github.com/berndporr/libcamera2opencv](github.com/berndporr/libcamera2opencv) 

We would also like to thank Nash for his contribution to the eye tracking code.
The original code can be found at [github.com/bsdnoobz/opencv-code/blob/master/eye-tracking.cpp](github.com/bsdnoobz/opencv-code/blob/master/eye-tracking.cpp) 

We would also like to thank Alessandro Ghedini for his contribution to the sound file player code.
The original code can be found at [https://gist.github.com/ghedo/963382](https://gist.github.com/ghedo/963382)

The contributors to this repository are [Vibhav Madhusudan](https://github.com/vibhavmadhusudhan99), [Hamirdhavalle DV](https://github.com/Hamirdhavalle-dv), [Adebibawuah](https://github.com/Adebibawuah) and [Adrian Dsilva](https://github.com/adrian-7979)

The TaskBoard of the project can be found in the Projects tab of this repository.

### Social media links:
[Instagram](https://www.instagram.com/zappsnap_ug/?igsh=MWh4eWdqcnAxMWRkYQ%3D%3D)

## Prerequisites

```
apt install libopencv-dev libcamera-dev
```

This program also uses ALSA to play the alarm sound.
```
sudo apt-get install libasound2-dev
```
Additionally, pigpio also has to be downloaded from [here](https://abyz.me.uk/rpi/pigpio/index.html), for all GPIO functions. 

## Compilation and installation

```
cmake .
make
sudo make install
```

## Output File
The output file "eye" is in the subdirectory "eye-monitor", and can be run
in Linux terminal with the command, after changing the directory to "eye-monitor"
```
cd eye-monitor
sudo ./eye
```
The output file can also be made to run at start-up using instructions shown [here](https://www.tutorialspoint.com/run-a-script-on-startup-in-linux#:~:text=Make%20the%20script%20file%20executable,scriptname%20defaults"%20in%20the%20terminal.)

