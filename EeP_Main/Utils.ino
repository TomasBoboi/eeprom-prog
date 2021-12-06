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
    static char addressAsString_ac[7];

    addressAsString_ac[0] = '0';
    addressAsString_ac[1] = 'x';
    addressAsString_ac[2] = Utils_DigitToHexChar((address_u16 >> 12) & 0x000F);
    addressAsString_ac[3] = Utils_DigitToHexChar((address_u16 >> 8) & 0x000F);
    addressAsString_ac[4] = Utils_DigitToHexChar((address_u16 >> 4) & 0x000F);
    addressAsString_ac[5] = Utils_DigitToHexChar(address_u16 & 0x000F);

    addressAsString_ac[6] = '\0';

    return addressAsString_ac;
}

char *Utils_ByteToHexString(uint8_t byte_u8)
{
    static char byteAsString_ac[5];

    byteAsString_ac[0] = '0';
    byteAsString_ac[1] = 'x';
    byteAsString_ac[2] = Utils_DigitToHexChar((byte_u8 >> 4) & 0x0F);
    byteAsString_ac[3] = Utils_DigitToHexChar(byte_u8 & 0x0F);

    byteAsString_ac[4] = '\0';

    return byteAsString_ac;
}

char *Utils_FourBytesToString(uint8_t bytes_au8[4])
{
    static char bytesAsString_ach[20];

    strcpy(bytesAsString_ach, Utils_ByteToHexString(bytes_au8[0]));
    for (uint8_t index_u8 = 1; index_u8 < 4; index_u8++)
    {
        strcat(bytesAsString_ach, " ");
        strcat(bytesAsString_ach, Utils_ByteToHexString(bytes_au8[index_u8]));
    }

    return bytesAsString_ach;
}

char *Utils_BlockToString(uint16_t startAddress_u16, uint8_t *block_pu8)
{
    static char blockAsString_ach[6 * EEPROM_BLOCK_SIZE_U16 + EEPROM_BLOCK_SIZE_U16 / 4 + 12];

    strcpy(blockAsString_ach, Utils_AddressToHexString(startAddress_u16));
    strcat(blockAsString_ach, ": ");

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_BLOCK_SIZE_U16; index_u8 += 4)
    {
        uint8_t fourBytesAsArray_au8[4];
        fourBytesAsArray_au8[0] = block_pu8[index_u8 + 0];
        fourBytesAsArray_au8[1] = block_pu8[index_u8 + 1];
        fourBytesAsArray_au8[2] = block_pu8[index_u8 + 2];
        fourBytesAsArray_au8[3] = block_pu8[index_u8 + 3];

        char *fourBytesAsString_pc = Utils_FourBytesToString(fourBytesAsArray_au8);
        strcat(blockAsString_ach, fourBytesAsString_pc);
        strcat(blockAsString_ach, "  ");
    }
    
    strcat(blockAsString_ach, "|  ");

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_BLOCK_SIZE_U16; index_u8++)
    {
        char dataAsChar_ach[2] = {'.', '\0'};

        if(isPrintable(block_pu8[index_u8]))
        {
            dataAsChar_ach[0] = block_pu8[index_u8];
        }

        strcat(blockAsString_ach, dataAsChar_ach);
    }

    return blockAsString_ach;
}

uint32_t Utils_GetNumberFromSerial(const char * tag_pch)
{
    char number_str[32] = {0};
    char currentChar_ch;
    uint8_t index_u8 = 0;

    Serial.println();
    Serial.print(tag_pch);

    while(1)
    {
        while(Serial.available() == 0);

        currentChar_ch = Serial.read();
        if('\r' != currentChar_ch && '\n' != currentChar_ch)
        {
            Serial.print(currentChar_ch);
            if('\b' == currentChar_ch && 0 != index_u8)
            {
                index_u8--;
            }
            else
            {
                number_str[index_u8++] = currentChar_ch;
            }
        }
        else
        {
            break;
        }
    }
    
    return (uint32_t)strtol(number_str, 0, 0);
}

void Utils_PrintMenu()
{
    Serial.println();
    Serial.println("┌─────────────────────────┐");
    Serial.println("│ 1. Read byte            │");
    Serial.println("│ 2. Read block           │");
    Serial.println("│ 3. Read N blocks        │");
    Serial.println("│ 4. Write byte           │");
    Serial.println("│ 5. Write block          │");
    Serial.println("│ 6. Write N blocks       │");
    Serial.println("│ 7. Erase byte           │");
    Serial.println("│ 8. Erase block          │");
    Serial.println("│ 9. Erase N blocks       │");
    Serial.println("│ 10. Erase chip          │");
    Serial.println("└─────────────────────────┘");
}