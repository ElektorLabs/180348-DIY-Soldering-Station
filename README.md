#  180348 / 150500 - Soldering station
Software for the soldering station ( 180348 / 150500 ) based on the arduino framwork.

Shows the Temperature in °C and is limited to use 1.5Amp of current.

## Getting Started

Download the source and open it with the arduino ide ( >= 1.8.x ) and select Arduino Leonardo as target.
The station shall be detected via usb as serial port. Set the ide to use the port and compile the sketch.
If the leonardo bootloader is working you can just press upload and the software will be transfered to the station.
Be aware that after flashing the new software, your current temperature settings may be gone and set back to 50°C


### Prerequisites

You need TimerOne library installed in your libs path.


### Serialinterface ( since firmware 1.2 )

The USB-connector on the station works as serial device. If you plug it in your computer you can than access
the serial port with the terminal form the arduino ide or putty / puttytel or any other serial terminal. If 
you are forced to set a baudrate choose 115200, as this is serial over usb the device basically don't care. 
As we have the leonardo bootloader on the chip don't user 1200 baud, as this may triggers the bootlloader. 

##### Supported commands of the console

* set/get setpoint [xxx] -> This will set or get the setpoint
* get temperature        -> This will report the current temperature
* clear error            -> If an error is shown this will clear it
* help                   -> This will show the help
 
