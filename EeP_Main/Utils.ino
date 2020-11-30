#include "EeP_ChipParams.h"
#include "Utils.h"

char Utils_DigitToHexChar(uint8_t digit_u8)
{
    char result_c;

    if (digit_u8 >= 0 && digit_u8 < 10)
    {
        result_c = digit_u8 + '0';
    }
    else if (digit_u8 >= 10 && digit_u8 < 16)
    {
        result_c = digit_u8 - 10 + 'A';
    }
    else
    {
        result_c = '0';
    }

    return result_c;
}

uint8_t Utils_HexCharToDigit(char hexDigit_c)
{
    uint8_t digit_u8;

    if (hexDigit_c >= '0' && hexDigit_c <= '9')
        digit_u8 = hexDigit_c - '0';
    else if (hexDigit_c >= 'A' && hexDigit_c <= 'F')
        digit_u8 = hexDigit_c - 'A' + 10;
    else if (hexDigit_c >= 'a' && hexDigit_c <= 'f')
        digit_u8 = Utils_HexCharToDigit(hexDigit_c - 'a' + 'A');
    else
        digit_u8 = 0;

    return digit_u8;
}

char *Utils_AddressToHexString(uint16_t address_u16)
{
    static char addressAsString_ac[9];

    addressAsString_ac[0] = '0';
    addressAsString_ac[1] = 'x';
    addressAsString_ac[2] = Utils_DigitToHexChar((address_u16 >> 12) & 0x000F);
    addressAsString_ac[3] = Utils_DigitToHexChar((address_u16 >> 8) & 0x000F);
    addressAsString_ac[4] = Utils_DigitToHexChar((address_u16 >> 4) & 0x000F);
    addressAsString_ac[5] = Utils_DigitToHexChar(address_u16 & 0x000F);

    addressAsString_ac[6] = ':';
    addressAsString_ac[7] = ' ';

    addressAsString_ac[8] = '\0';

    return addressAsString_ac;
}

char *Utils_ByteToHexString(uint8_t byte_u8)
{
    static char byteAsString_ac[3];

    byteAsString_ac[0] = Utils_DigitToHexChar((byte_u8 >> 4) & 0x0F);
    byteAsString_ac[1] = Utils_DigitToHexChar(byte_u8 & 0x0F);

    byteAsString_ac[2] = '\0';

    return byteAsString_ac;
}

char *Utils_FourBytesToString(uint8_t bytes_au8[4])
{
    static char bytesAsString_ac[12];

    strcpy(bytesAsString_ac, Utils_ByteToHexString(bytes_au8[0]));
    for (uint8_t index_u8 = 1; index_u8 < 4; index_u8++)
    {
        strcat(bytesAsString_ac, " ");
        strcat(bytesAsString_ac, Utils_ByteToHexString(bytes_au8[index_u8]));
    }

    return bytesAsString_ac;
}

char *Utils_BlockToString(uint16_t startAddress_u16, uint8_t *block_pu8)
{
    static char blockAsString_pc[3 * EEPROM_BLOCK_SIZE + EEPROM_BLOCK_SIZE / 4 + 7];

    strcpy(blockAsString_pc, Utils_AddressToHexString(startAddress_u16));

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_BLOCK_SIZE; index_u8 += 4)
    {
        uint8_t fourBytesAsArray_au8[4];
        fourBytesAsArray_au8[0] = block_pu8[index_u8 + 0];
        fourBytesAsArray_au8[1] = block_pu8[index_u8 + 1];
        fourBytesAsArray_au8[2] = block_pu8[index_u8 + 2];
        fourBytesAsArray_au8[3] = block_pu8[index_u8 + 3];

        char *fourBytesAsString_pc = Utils_FourBytesToString(fourBytesAsArray_au8);
        strcat(blockAsString_pc, fourBytesAsString_pc);

        if (index_u8 + 4 != EEPROM_BLOCK_SIZE)
            strcat(blockAsString_pc, "  ");
    }

    return blockAsString_pc;
}

uint16_t Utils_GetAddressFromSerial()
{
    Serial.println();
    Serial.print("Address: ");

    while (Serial.available() < 6)
        ;

    char addressAsString_ca[7];
    for (uint8_t index_u8 = 0; index_u8 < 6; index_u8++)
    {
        addressAsString_ca[index_u8] = Serial.read();
    }
    addressAsString_ca[6] = '\0';

    uint16_t address_u16 = Utils_AddressFromHexString(addressAsString_ca);

    Serial.print(addressAsString_ca);
    Serial.println();

    return address_u16;
}

uint8_t Utils_GetByteFromSerial()
{
    Serial.print("Data: ");

    while(Serial.available() < 2);

    uint8_t byte_u8 = 0x00;
    byte_u8 = (byte_u8 | Utils_HexCharToDigit((char)Serial.read())) << 4;
    byte_u8 = byte_u8 | Utils_HexCharToDigit((char)Serial.read());

    return byte_u8;
}

uint16_t Utils_AddressFromHexString(char addressAsString[7])
{
    uint16_t address_u16 = 0x0000;

    address_u16 = (address_u16 | Utils_HexCharToDigit(addressAsString[2])) << 4;
    address_u16 = (address_u16 | Utils_HexCharToDigit(addressAsString[3])) << 4;
    address_u16 = (address_u16 | Utils_HexCharToDigit(addressAsString[4])) << 4;
    address_u16 = (address_u16 | Utils_HexCharToDigit(addressAsString[5]));

    return address_u16;
}

void Utils_PrintMenu()
{
    Serial.println();
    Serial.println("┌─────────────────────────┐");
    Serial.println("│ 1. Read byte            │");
    Serial.println("│ 2. Read block           │");
    Serial.println("│ 3. Write byte           │");
    Serial.println("│ 4. Write block          │");
    Serial.println("│ 5. Erase chip           │");
    Serial.println("└─────────────────────────┘");
    Serial.print("Choice: ");
}