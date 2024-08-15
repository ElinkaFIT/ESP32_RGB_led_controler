# ESP32 RGB and LED BLE controler
This project focuses on creating code for the ESP32 microcontroller to control groups of LED diodes and an RGB LED via a Bluetooth interface.

## Project Overview
The ESP32 microcontroller in this project serves as a server that receives data from a mobile-optimized web page through a Bluetooth interface. The web page includes buttons for pairing with the microcontroller, starting various LED animations, and selecting one of three speed modes for the animations, with the slowest speed set by default. The speed can be adjusted even while animations are running.

The project features several light animations created using three LEDs and one RGB LED. These animations simulate various real-world scenarios, such as countdown timers and turn signals. Additionally, there are RGB-specific animations like traffic light simulation and color demonstration, including purple and turquoise.

##Technical Details
Programming Environment: The project was developed using the IDF and Visual Studio Code with the PlatformIO extension.
Web Development: The web interface was built using HTML and JavaScript.
Hardware Setup: LEDs are connected to GPIO pins 12, 13, 14, and the RGB LED is connected to pins 17, 25, and 26 on the ESP32 board. Each LED is connected via a 330-ohm resistor.
Implementation
The implementation involves initializing non-volatile memory, GAP and GATT profiles for Bluetooth communication, and setting up the BLE configuration on the ESP32. The server communicates with the web application through specific codes, which trigger animations or adjust speed based on the received code. Animations are queued and processed in sequence, with the speed of the animations influenced by a global speed setting.

##Final Project
All components of the project were successfully implemented. A demonstration of the project can be viewed on YouTube:

[Project demonstration video](https://youtu.be/dr4dR24Nvzg)
