#include "EeP_ChipParams.h"
#include "Ser_Main.h"
#include "Utils.h"

/* ------------------------- <DEFINES> ------------------------- */
#define CHIP_N_OE 13
/* ------------------------- </DEFINES> ------------------------- */

/* ------------------------- <TYPES> ------------------------- */
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

void EeP_WriteByte(uint16_t address_u16, uint8_t dataByte_u8);
uint8_t EeP_ReadByte(uint16_t address_u16);

void EeP_WriteBlock(uint16_t startAddress_u16, uint8_t *dataBlock_pu8);
uint8_t * EeP_ReadBlock(uint16_t startAddress_u16);

void EeP_EraseByte(uint16_t address_u16);
void EeP_EraseBlock(uint16_t startAddress_u16);
void EeP_EraseNBlocks(uint16_t startAddress_u16, uint16_t numberOfBlocks_u16);
void EeP_EraseChip();
/* ------------------------- </FUNCTIONS> ------------------------- */

/* ------------------------- <IMPLEMENTATIONS> ------------------------- */
void EeP_Init()
{
    Ser_Init();

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_DATA_PINS_NO_U8; index_u8++)
    {
        pinMode(EeP_ChipDataPins_au8[index_u8], OUTPUT);
    }

    pinMode(CHIP_N_OE, OUTPUT);
    digitalWrite(CHIP_N_OE, HIGH);

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_DATA_PINS_NO_U8; index_u8++)
    {
        digitalWrite(EeP_ChipDataPins_au8[index_u8], LOW);
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

void EeP_WriteByte(uint16_t address_u16, uint8_t dataByte_u8)
{
    if(address_u16 < EEPROM_START_ADDRESS_U16 || address_u16 > EEPROM_END_ADDRESS_U16)
    {
        /* some error */
    }
    else
    {
        EeP_WriteAddressAndWrE(address_u16, LOW);

        EeP_WriteDataOutputs(dataByte_u8);

        delay(EEPROM_WRITE_DELAY_U32);

        EeP_WriteAddressAndWrE(address_u16, HIGH);
    }
}

uint8_t EeP_ReadByte(uint16_t address_u16)
{
    uint8_t data_u8 = 0xFF;

    if(address_u16 < EEPROM_START_ADDRESS_U16 || address_u16 > EEPROM_END_ADDRESS_U16)
    {
        /* some error */
    }
    else
    {
        EeP_WriteAddressAndWrE(address_u16, HIGH);
        data_u8 = EeP_ReadDataOutputs();
    }

    return data_u8;
}

void EeP_WriteBlock(uint16_t startAddress_u16, uint8_t *dataBlock_pu8)
{
    if(NULL == dataBlock_pu8 || startAddress_u16 < EEPROM_START_ADDRESS_U16 || startAddress_u16 > EEPROM_END_ADDRESS_U16)
    {
        /* some error */
    }
    else
    {
        for (uint16_t currentAddress_u16 = startAddress_u16; currentAddress_u16 < startAddress_u16 + EEPROM_BLOCK_SIZE_U16 && currentAddress_u16 <= EEPROM_END_ADDRESS_U16; currentAddress_u16++)
        {
            EeP_WriteByte(currentAddress_u16, dataBlock_pu8[currentAddress_u16 - startAddress_u16]);
        }
    }
}

uint8_t * EeP_ReadBlock(uint16_t startAddress_u16)
{
    static uint8_t blockData_au8[EEPROM_BLOCK_SIZE_U16];

    if(startAddress_u16 < EEPROM_START_ADDRESS_U16 || startAddress_u16 > EEPROM_END_ADDRESS_U16)
    {
        /* some error */
    }
    else
    {
        for(uint16_t currentAddress_u16 = startAddress_u16; currentAddress_u16 < startAddress_u16 + EEPROM_BLOCK_SIZE_U16; currentAddress_u16++)
        {
            blockData_au8[currentAddress_u16 - startAddress_u16] = EeP_ReadByte(currentAddress_u16);
        }
    }

    return blockData_au8;
}

void EeP_EraseByte(uint16_t address_u16)
{
    EeP_WriteByte(address_u16, EEPROM_ERASED_BYTE_VALUE_U8);
}

void EeP_EraseBlock(uint16_t startAddress_u16)
{
    for(uint16_t currentAddress_u16 = startAddress_u16; currentAddress_u16 < startAddress_u16 + EEPROM_BLOCK_SIZE_U16; currentAddress_u16++)
    {
        EeP_EraseByte(startAddress_u16);
    }
}

void EeP_EraseNBlocks(uint16_t startAddress_u16, uint16_t numberOfBlocks_u16)
{
    for(uint16_t blockIndex_u16 = 0; blockIndex_u16 < numberOfBlocks_u16; blockIndex_u16++)
    {
        EeP_EraseBlock(startAddress_u16 + blockIndex_u16 * EEPROM_BLOCK_SIZE_U16);
    }
}

void EeP_EraseChip()
{
    EeP_EraseNBlocks(EEPROM_START_ADDRESS_U16, EEPROM_TOTAL_NUMBER_OF_BLOCKS_U16);
}
/* ------------------------- </IMPLEMENTATIONS> ------------------------- */

void setup()
{
    EeP_Init();
}

void loop()
{
    uint32_t menuChoice_u32 = Utils_GetNumberFromSerial("Choice: ");
    uint32_t numberOfBlocks_u32;
    uint16_t address_u16;
    uint8_t dataByte_u8;

    switch (menuChoice_u32)
    {
    case MENU_CHOICE_READ_BYTE:
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");

        if(address_u16 >= EEPROM_START_ADDRESS_U16 && address_u16 <= EEPROM_END_ADDRESS_U16)
        {
            Serial.println();
            Serial.print(Utils_AddressToHexString(address_u16));
            Serial.print(": ");
            Serial.println(Utils_ByteToHexString(EeP_ReadByte(address_u16)));
        }
        else
        {
            Serial.print("ERROR: ");
            Serial.print(Utils_AddressToHexString(address_u16));
            Serial.println(" exceeds the chip's address space!");
        }
        break;

    case MENU_CHOICE_READ_BLOCK:
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");

        if(address_u16 >= EEPROM_START_ADDRESS_U16 && address_u16 <= EEPROM_END_ADDRESS_U16)
        {
            Serial.println();
            Serial.println(Utils_BlockToString(address_u16, EeP_ReadBlock(address_u16)));
        }
        else
        {
            Serial.print("ERROR: ");
            Serial.print(Utils_AddressToHexString(address_u16));
            Serial.println(" exceeds the chip's address space!");
        }
        break;

    case MENU_CHOICE_READ_N_BLOCKS:
        numberOfBlocks_u32 = Utils_GetNumberFromSerial("Number of blocks: ");
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");

        Serial.println();
        for (uint32_t index_u32 = 0; index_u32 < numberOfBlocks_u32; index_u32++)
        {
            Serial.println(Utils_BlockToString(address_u16, EeP_ReadBlock(address_u16)));

            address_u16 += EEPROM_BLOCK_SIZE_U16;
        }
        break;

    case MENU_CHOICE_WRITE_BYTE:
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");
        dataByte_u8 = (uint8_t)Utils_GetNumberFromSerial("Data ([0x00-0xFF]/[0-255]): ");

        EeP_WriteByte(address_u16, dataByte_u8);
        Serial.println();
        Serial.println("Written successfully");
        break;

    case MENU_CHOICE_WRITE_BLOCK:
        address_u16 = (uint16_t)Utils_GetNumberFromSerial("Address (0x____): ");

        for (uint8_t index_u8 = 0; index_u8 < EEPROM_BLOCK_SIZE_U16; index_u8++)
        {
            dataByte_u8 = (uint8_t)Utils_GetNumberFromSerial("Data ([0x00-0xFF]/[0-255]): ");
            EeP_WriteByte(address_u16, dataByte_u8);

            if (address_u16 + 1 < EEPROM_END_ADDRESS_U16)
                address_u16++;
        }
        Serial.println();
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
            EeP_EraseByte(address_u16);

            while (Serial.available() > 0)
                (void)Serial.read();
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