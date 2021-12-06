#include "EeP_ChipParams.h"
#include "Ser_Main.h"
#include "Utils.h"

/* ------------------------- <DEFINES> ------------------------- */
#define CHIP_N_OE 13
/* ------------------------- </DEFINES> ------------------------- */

/* ------------------------- <TYPES> ------------------------- */
typedef enum
{
    STATUS_OK,
    STATUS_NOT_OK
} EeP_ReturnStatus_en;

typedef enum
{
    CHIP_DATA_PIN_0 = 5,
    CHIP_DATA_PIN_1,
    CHIP_DATA_PIN_2,
    CHIP_DATA_PIN_3,
    CHIP_DATA_PIN_4,
    CHIP_DATA_PIN_5,
    CHIP_DATA_PIN_6,
    CHIP_DATA_PIN_7
} EeP_ChipDataPins_en;

typedef enum
{
    MENU_CHOICE_BEGIN,

    MENU_CHOICE_READ_BYTE,
    MENU_CHOICE_READ_BLOCK,
    MENU_CHOICE_READ_N_BLOCKS,

    MENU_CHOICE_WRITE_BYTE,
    MENU_CHOICE_WRITE_BLOCK,
    MENU_CHOICE_WRITE_N_BLOCKS,

    MENU_CHOICE_ERASE_BYTE,
    MENU_CHOICE_ERASE_BLOCK,
    MENU_CHOICE_ERASE_CHIP,

    MENU_CHOICE_END
} EeP_MenuChoice_en;
/* ------------------------- </TYPES> ------------------------- */

/* ------------------------- <VARIABLES> ------------------------- */
const uint8_t EeP_ChipDataPins_au8[EEPROM_DATA_PINS_NO_U8] =
    {CHIP_DATA_PIN_0,
     CHIP_DATA_PIN_1,
     CHIP_DATA_PIN_2,
     CHIP_DATA_PIN_3,
     CHIP_DATA_PIN_4,
     CHIP_DATA_PIN_5,
     CHIP_DATA_PIN_6,
     CHIP_DATA_PIN_7};
/* ------------------------- </VARIABLES> ------------------------- */

/* ------------------------- <FUNCTIONS> ------------------------- */
void EeP_Init();
void EeP_WriteAddressAndWrE(uint16_t address_u16, uint8_t nWriteEnable_u8);

void EeP_WriteDataOutputs(uint8_t data_u8);
uint8_t EeP_ReadDataOutputs();

EeP_ReturnStatus_en EeP_WriteByte(uint16_t address_u16, uint8_t dataByte_u8);
EeP_ReturnStatus_en EeP_ReadByte(uint16_t address_u16, uint8_t * dataByte_pu8);

EeP_ReturnStatus_en EeP_WriteBlock(uint16_t startAddress_u16, uint8_t *dataBlock_pu8);
EeP_ReturnStatus_en EeP_ReadBlock(uint16_t startAddress_u16, uint8_t * dataBlock_pu8);

EeP_ReturnStatus_en EeP_EraseByte(uint16_t address_u16);
EeP_ReturnStatus_en EeP_EraseBlock(uint16_t startAddress_u16);
EeP_ReturnStatus_en EeP_EraseNBlocks(uint16_t startAddress_u16, uint16_t numberOfBlocks_u16);
EeP_ReturnStatus_en EeP_EraseChip();
/* ------------------------- </FUNCTIONS> ------------------------- */

/* ------------------------- <IMPLEMENTATIONS> ------------------------- */
void EeP_Init()
{
    Ser_Init();

    pinMode(CHIP_N_OE, OUTPUT);
    digitalWrite(CHIP_N_OE, HIGH);

    EeP_WriteAddressAndWrE((uint16_t)0x0000, HIGH);

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_DATA_PINS_NO_U8; index_u8++)
    {
        pinMode(EeP_ChipDataPins_au8[index_u8], INPUT);
    }

    Utils_PrintMenu();
}

void EeP_WriteAddressAndWrE(uint16_t address_u16, uint8_t nWriteEnable_u8)
{
    uint16_t dataToBeSent_u16 = 0x0000;

    dataToBeSent_u16 = (address_u16 & 0x7FFF);
    dataToBeSent_u16 = dataToBeSent_u16 | (nWriteEnable_u8 << 15);

    Ser_SendSerialWord(dataToBeSent_u16);
}

void EeP_WriteDataOutputs(uint8_t data_u8)
{
    digitalWrite(CHIP_N_OE, HIGH);

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_DATA_PINS_NO_U8; index_u8++)
    {
        pinMode(EeP_ChipDataPins_au8[index_u8], OUTPUT);
    }

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_DATA_PINS_NO_U8; index_u8++)
    {
        digitalWrite(EeP_ChipDataPins_au8[index_u8], (data_u8 >> index_u8) & 0x01);
    }
}

uint8_t EeP_ReadDataOutputs()
{
    for (uint8_t index_u8 = 0; index_u8 < EEPROM_DATA_PINS_NO_U8; index_u8++)
    {
        pinMode(EeP_ChipDataPins_au8[index_u8], INPUT);
    }

    digitalWrite(CHIP_N_OE, LOW);

    uint8_t readData_u8 = 0x00;
    for (int8_t index_u8 = EEPROM_DATA_PINS_NO_U8 - 1; index_u8 >= 0; index_u8--)
    {
        if (index_u8 != 0)
        {
            readData_u8 = (readData_u8 | digitalRead(EeP_ChipDataPins_au8[index_u8])) << 1;
        }
        else
        {
            readData_u8 |= digitalRead(EeP_ChipDataPins_au8[index_u8]);
        }
    }

    return readData_u8;
}

EeP_ReturnStatus_en EeP_WriteByte(uint16_t address_u16, uint8_t dataByte_u8)
{
    EeP_ReturnStatus_en returnStatus_en = STATUS_OK;

    if(address_u16 < EEPROM_START_ADDRESS_U16 || address_u16 > EEPROM_END_ADDRESS_U16)
    {
        returnStatus_en = STATUS_NOT_OK;
    }
    else
    {
        EeP_WriteAddressAndWrE(address_u16, LOW);

        EeP_WriteDataOutputs(dataByte_u8);

        delay(EEPROM_WRITE_DELAY_U32);

        EeP_WriteAddressAndWrE(address_u16, HIGH);
    }

    return returnStatus_en;
}

EeP_ReturnStatus_en EeP_ReadByte(uint16_t address_u16, uint8_t * dataByte_pu8)
{
    EeP_ReturnStatus_en returnStatus_en = STATUS_OK;
    *dataByte_pu8 = EEPROM_ERASED_BYTE_VALUE_U8;

    if(NULL == dataByte_pu8 || address_u16 < EEPROM_START_ADDRESS_U16 || address_u16 > EEPROM_END_ADDRESS_U16)
    {
        returnStatus_en = STATUS_NOT_OK;
    }
    else
    {
        EeP_WriteAddressAndWrE(address_u16, HIGH);
        *dataByte_pu8 = EeP_ReadDataOutputs();
    }

    return returnStatus_en;
}

EeP_ReturnStatus_en EeP_WriteBlock(uint16_t startAddress_u16, uint8_t *dataBlock_pu8)
{
    EeP_ReturnStatus_en returnStatus_en = STATUS_OK;

    if(NULL == dataBlock_pu8 || startAddress_u16 < EEPROM_START_ADDRESS_U16 || startAddress_u16 > EEPROM_END_ADDRESS_U16)
    {
        returnStatus_en = STATUS_NOT_OK;
    }
    else
    {
        for (uint16_t currentAddress_u16 = startAddress_u16; currentAddress_u16 < startAddress_u16 + EEPROM_BLOCK_SIZE_U16 && currentAddress_u16 <= EEPROM_END_ADDRESS_U16; currentAddress_u16++)
        {
            returnStatus_en = EeP_WriteByte(currentAddress_u16, dataBlock_pu8[currentAddress_u16 - startAddress_u16]);

            if(STATUS_NOT_OK == returnStatus_en)
            {
                break;
            }
        }
    }

    return returnStatus_en;
}

EeP_ReturnStatus_en EeP_ReadBlock(uint16_t startAddress_u16, uint8_t * dataBlock_pu8)
{
    EeP_ReturnStatus_en returnStatus_en = STATUS_OK;

    for(uint16_t index_u16 = 0; index_u16 < EEPROM_BLOCK_SIZE_U16; index_u16++)
    {
        dataBlock_pu8[index_u16] = EEPROM_ERASED_BYTE_VALUE_U8;
    }

    if(NULL == dataBlock_pu8 || startAddress_u16 < EEPROM_START_ADDRESS_U16 || startAddress_u16 > EEPROM_END_ADDRESS_U16)
    {
        returnStatus_en = STATUS_NOT_OK;
    }
    else
    {
        for(uint16_t currentAddress_u16 = startAddress_u16; currentAddress_u16 < startAddress_u16 + EEPROM_BLOCK_SIZE_U16; currentAddress_u16++)
        {
            returnStatus_en = EeP_ReadByte(currentAddress_u16, dataBlock_pu8 + currentAddress_u16 - startAddress_u16);

            if(STATUS_NOT_OK == returnStatus_en)
            {
                break;
            }
        }
    }

    return returnStatus_en;
}

EeP_ReturnStatus_en EeP_EraseByte(uint16_t address_u16)
{
    EeP_ReturnStatus_en returnStatus_en = STATUS_OK;

    returnStatus_en = EeP_WriteByte(address_u16, EEPROM_ERASED_BYTE_VALUE_U8);

    return returnStatus_en;
}

EeP_ReturnStatus_en EeP_EraseBlock(uint16_t startAddress_u16)
{
    EeP_ReturnStatus_en returnStatus_en = STATUS_OK;

    for(uint16_t currentAddress_u16 = startAddress_u16; currentAddress_u16 < startAddress_u16 + EEPROM_BLOCK_SIZE_U16; currentAddress_u16++)
    {
        returnStatus_en = EeP_EraseByte(startAddress_u16);

        if(STATUS_NOT_OK == returnStatus_en)
        {
            break;
        }
    }

    return returnStatus_en;
}

EeP_ReturnStatus_en EeP_EraseNBlocks(uint16_t startAddress_u16, uint16_t numberOfBlocks_u16)
{
    EeP_ReturnStatus_en returnStatus_en = STATUS_OK;

    for(uint16_t blockIndex_u16 = 0; blockIndex_u16 < numberOfBlocks_u16; blockIndex_u16++)
    {
        returnStatus_en = EeP_EraseBlock(startAddress_u16 + blockIndex_u16 * EEPROM_BLOCK_SIZE_U16);

        if(STATUS_NOT_OK == returnStatus_en)
        {
            break;
        }
    }

    return returnStatus_en;
}

EeP_ReturnStatus_en EeP_EraseChip()
{
    EeP_ReturnStatus_en returnStatus_en = STATUS_OK;

    returnStatus_en = EeP_EraseNBlocks(EEPROM_START_ADDRESS_U16, EEPROM_TOTAL_NUMBER_OF_BLOCKS_U16);

    return returnStatus_en;
}
/* ------------------------- </IMPLEMENTATIONS> ------------------------- */

void setup()
{
    EeP_Init();
}

void loop()
{
    EeP_ReturnStatus_en returnStatus_en = STATUS_OK;
    uint32_t numberOfBlocks_u32;
    uint16_t address_u16;
    uint8_t dataByte_u8;
    uint8_t dataBlock_au8[EEPROM_BLOCK_SIZE_U16];

    uint32_t menuChoice_u32 = Utils_GetNumberFromSerial("Choice: ");

    switch (menuChoice_u32)
    {
    case MENU_CHOICE_READ_BYTE:
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");
        returnStatus_en = EeP_ReadByte(address_u16, &dataByte_u8);
        Serial.println();

        if(STATUS_NOT_OK == returnStatus_en)
        {
            Serial.println("ERROR: The provided address is outside the chip's address space!");
        }
        else
        {
            Serial.print(Utils_AddressToHexString(address_u16));
            Serial.print(": ");
            Serial.println(Utils_ByteToHexString(dataByte_u8));
        }
        break;

    case MENU_CHOICE_READ_BLOCK:
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");
        returnStatus_en = EeP_ReadBlock(address_u16, &dataBlock_au8[0]);
        Serial.println();

        if(STATUS_NOT_OK == returnStatus_en)
        {
            Serial.println("ERROR: Some addresses are outside the chip's address space!");
        }

        Serial.println(Utils_BlockToString(address_u16, &dataBlock_au8[0]));
        break;

    case MENU_CHOICE_READ_N_BLOCKS:
        numberOfBlocks_u32 = Utils_GetNumberFromSerial("Number of blocks: ");
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");
        Serial.println();

        for (uint32_t blockIndex_u32 = 0; blockIndex_u32 < numberOfBlocks_u32; blockIndex_u32++)
        {
            returnStatus_en = EeP_ReadBlock(address_u16, &dataBlock_au8[0]);

            if(STATUS_NOT_OK == returnStatus_en)
            {
                Serial.println("ERROR: Some addresses are outside the chip's address space!");
            }

            Serial.println(Utils_BlockToString(address_u16, &dataBlock_au8[0]));

            if(STATUS_NOT_OK == returnStatus_en)
            {
                break;
            }

            address_u16 += EEPROM_BLOCK_SIZE_U16;
        }
        break;

    case MENU_CHOICE_WRITE_BYTE:
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");
        dataByte_u8 = (uint8_t)Utils_GetNumberFromSerial("Data ([0x00-0xFF]/[0-255]): ");

        returnStatus_en = EeP_WriteByte(address_u16, dataByte_u8);

        if(STATUS_NOT_OK == returnStatus_en)
        {
            Serial.println();
            Serial.println("ERROR: The provided address is outside the chip's address space!");
        }
        else
        {
            Serial.println();
            Serial.println("Written successfully!");
        }
        break;

    case MENU_CHOICE_WRITE_BLOCK:
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");

        for (uint8_t index_u8 = 0; index_u8 < EEPROM_BLOCK_SIZE_U16; index_u8++)
        {
            char tag_ach[33] = "Data[00] ([0x00-0xFF]/[0-255]): ";
            tag_ach[5] = '0' + index_u8 / 10;
            tag_ach[6] = '0' + index_u8 % 10;

            dataBlock_au8[index_u8] = (uint8_t)Utils_GetNumberFromSerial(tag_ach);
        }

        returnStatus_en = EeP_WriteBlock(address_u16, dataBlock_au8);

        if(STATUS_NOT_OK == returnStatus_en)
        {
            Serial.println();
            Serial.println("ERROR: Some addresses are outside the chip's address space!");
        }
        else
        {
            Serial.println();
            Serial.println("Written successfully!");
        }
        break;
    
    case MENU_CHOICE_WRITE_N_BLOCKS:
        numberOfBlocks_u32 = Utils_GetNumberFromSerial("Number of blocks: ");
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");

        Serial.println();
        for (uint32_t blockIndex_u32 = 0; blockIndex_u32 < numberOfBlocks_u32; blockIndex_u32++)
        {
            for (uint8_t index_u8 = 0; index_u8 < EEPROM_BLOCK_SIZE_U16; index_u8++)
            {
                dataByte_u8 = (uint8_t)Utils_GetNumberFromSerial("Data ([0x00-0xFF]/[0-255]): ");
                EeP_WriteByte(address_u16, dataByte_u8);

                if (address_u16 + 1 < EEPROM_END_ADDRESS_U16)
                    address_u16++;
            }
        }
        break;
    
    case MENU_CHOICE_ERASE_BYTE:
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");

        Serial.println();
        Serial.println("This will lead to permanent data loss! Are you sure you wish to continue? (y/n) ");
        while (Serial.available() == 0)
            ;

        if (Serial.read() == 'y')
        {
            Serial.println("Erasing...");

            returnStatus_en = EeP_EraseByte(address_u16);

            while (Serial.available() > 0)
                (void)Serial.read();
            
            if(STATUS_NOT_OK == returnStatus_en)
            {
                Serial.println();
                Serial.println("ERROR: The provided address is outside the chip's address space!");
            }
            else
            {
                Serial.println();
                Serial.println("Erased successfully!");
            }
        }
        break;

    case MENU_CHOICE_ERASE_BLOCK:
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");

        Serial.println();
        Serial.println("This will lead to permanent data loss! Are you sure you wish to continue? (y/n) ");
        while (Serial.available() == 0)
            ;

        if (Serial.read() == 'y')
        {
            Serial.println("Erasing...");
            EeP_EraseBlock(address_u16);

            while (Serial.available() > 0)
                (void)Serial.read();
        }
        break;

    case MENU_CHOICE_ERASE_CHIP:
        Serial.println();
        Serial.println("This will lead to permanent data loss! Are you sure you wish to continue? (y/n) ");
        while (Serial.available() == 0)
            ;

        if (Serial.read() == 'y')
        {
            Serial.println("Erasing...");
            EeP_EraseChip();

            while (Serial.available() > 0)
                (void)Serial.read();
        }
        break;

    default:
        Serial.println("ERROR: Invalid choice!");
        break;
    }
}