# eeprom-prog
Arduino-based EEPROM programmer, based on Ben Eater's idea

Works with AT28C256-15PU EEPROM, using two SN74HC595N shift registers to transmit the ~WriteEnable and address signals.

    Future additions include:
    - code comments
    - ability to read (and possibly erase using a UV LED) M27256 EPROM
    - various read/write features, which might be useful