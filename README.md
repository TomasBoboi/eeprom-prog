# eeprom-prog
Arduino-based EEPROM programmer, based on Ben Eater's idea

Works with AT28C256-15PU EEPROM, using two SN74HC595N shift registers to transmit the ~OutputEnable and address signals.

I added a little menu to allow for more flexibility.

Works with 57200 baud serial communication.

Will be used in my 8086-based PC build, to program the software of the system into the EEPROM chip.
