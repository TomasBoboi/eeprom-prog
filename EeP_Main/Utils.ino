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