These files are used to build the AVR firmware to controll the Arcade buttons. The arcade buttons are programmed to work as USB keyboards by re-programming both chips of the ArduinoMega2560.
You will need an ISP programmer to get this to work. The ATMega8U2 firmware changes the default USB-Serial to an USB-Keyboard. The ATMega2560 is a quickly hacked up and stripped version of Marlin (which is overkill)

The ArduinoMega2560 was used because we have lots of spares at the office. It is by no means cost efficient.
