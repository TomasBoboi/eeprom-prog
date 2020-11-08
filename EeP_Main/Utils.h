#ifndef UTILS_H
#define UTILS_H

char Utils_DigitToHexChar(uint8_t digit_u8);
uint8_t Utils_HexCharToDigit(char hexDigit_c);

char *Utils_AddressToHexString(uint16_t address_u16);
char *Utils_ByteToHexString(uint8_t byte_u8);
char *Utils_FourBytesToString(uint8_t bytes_au8[4]);
char *Utils_BlockToString(uint16_t startAddress_u16, uint8_t *block_pu8);

uint16_t Utils_GetAddressFromSerial();
uint16_t Utils_AddressFromHexString(char addressAsString[7]);

void Utils_PrintMenu();

#endif