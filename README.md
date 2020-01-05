# NRF_Master_Slave_com
Bi-directional communication between two nrf 24 modules using arduino and usb/serial connection.
Code was taken and modified from these sources-

https://forum.arduino.cc/index.php?topic=396450.0

https://forum.arduino.cc/index.php?topic=421081.0

http://forcetronic.blogspot.com/2016/07/using-nrf24l01s-irq-pin-to-generate.html

and the python code is taken and modified from this site - 

https://medium.com/swlh/lets-write-a-chat-app-in-python-f6783a9ac170

# Usage
connect nrf modules to arduino appropiatelty. Connect interupt pins according  to the master and slave codes also change ce ancsn pin numbers if needed.
Upload master code to arduino and slave code to the other one.

in order to use the python script, 
connect the arduino to pc via usb
find out the com port number of the arduino
open a command promt into the directory of the script  and type python Serialport_gui.py --port COMx
,where x is the com port 
