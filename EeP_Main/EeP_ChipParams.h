#ifndef CHIP_PARAMS_H
#define CHIP_PARAMS_H

#define EEPROM_DATA_PINS_NO_U8       (uint8_t)8   /* pins */
#define EEPROM_BLOCK_SIZE_U16        (uint16_t)16 /* bytes */
#define EEPROM_WRITE_DELAY_U32       (uint32_t)3  /* milliseconds */
#define EEPROM_START_ADDRESS_U16     (uint16_t)0x0000
#define EEPROM_END_ADDRESS_U16       (uint16_t)0x7FFF
#define EEPROM_ERASED_BYTE_VALUE_U8  (uint8_t)0xFF

#endif