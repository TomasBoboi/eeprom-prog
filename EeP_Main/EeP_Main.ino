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
/* ------------------------- </TYPES> ------------------------- */

/* ------------------------- <VARIABLES> ------------------------- */
const uint8_t EeP_ChipDataPins_au8[EEPROM_DATA_PINS_NO] =
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

void EeP_WriteByte(uint16_t address_u16, uint8_t data_u8);
uint8_t EeP_ReadByte(uint16_t address_u16);

void EeP_WriteBlock(uint16_t address_u16, uint8_t *block_pu8);
uint8_t *EeP_ReadBlock(uint16_t offset_u16);

void EeP_EraseChip();
/* ------------------------- </FUNCTIONS> ------------------------- */

/* ------------------------- <IMPLEMENTATIONS> ------------------------- */
void EeP_Init()
{
    Ser_Init();

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_DATA_PINS_NO; index_u8++)
    {
        pinMode(EeP_ChipDataPins_au8[index_u8], OUTPUT);
    }

    pinMode(CHIP_N_OE, OUTPUT);
    digitalWrite(CHIP_N_OE, HIGH);

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_DATA_PINS_NO; index_u8++)
    {
        digitalWrite(EeP_ChipDataPins_au8[index_u8], LOW);
    }
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

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_DATA_PINS_NO; index_u8++)
    {
        pinMode(EeP_ChipDataPins_au8[index_u8], OUTPUT);
    }

    for (uint8_t index_u8 = 0; index_u8 < EEPROM_DATA_PINS_NO; index_u8++)
    {
        digitalWrite(EeP_ChipDataPins_au8[index_u8], (data_u8 >> index_u8) & 0x01);
    }
}

uint8_t EeP_ReadDataOutputs()
{
    for (uint8_t index_u8 = 0; index_u8 < EEPROM_DATA_PINS_NO; index_u8++)
    {
        pinMode(EeP_ChipDataPins_au8[index_u8], INPUT);
    }

    digitalWrite(CHIP_N_OE, LOW);

    uint8_t readData_u8 = 0x00;
    for (int8_t index_u8 = EEPROM_DATA_PINS_NO - 1; index_u8 >= 0; index_u8--)
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

void EeP_WriteByte(uint16_t address_u16, uint8_t data_u8)
{
    EeP_WriteAddressAndWrE(address_u16, LOW);

    EeP_WriteDataOutputs(data_u8);

    delay(EEPROM_WRITE_DELAY);

    EeP_WriteAddressAndWrE(address_u16, HIGH);
}

uint8_t EeP_ReadByte(uint16_t address_u16)
{
    uint8_t readResult_u8;

    EeP_WriteAddressAndWrE(address_u16, HIGH);

    readResult_u8 = EeP_ReadDataOutputs();
    return readResult_u8;
}

void EeP_WriteBlock(uint16_t address_u16, uint8_t *block_pu8)
{
    for (uint16_t index_u16 = address_u16; index_u16 < address_u16 + EEPROM_BLOCK_SIZE && index_u16 <= EEPROM_END_ADDRESS; index_u16++)
    {
        EeP_WriteByte(index_u16, block_pu8[index_u16 - address_u16]);
    }
}

uint8_t *EeP_ReadBlock(uint16_t offset_u16)
{
    static uint8_t blockContent_au8[EEPROM_BLOCK_SIZE] = {0};

    for (uint16_t index_u16 = offset_u16; index_u16 < offset_u16 + EEPROM_BLOCK_SIZE && index_u16 <= EEPROM_END_ADDRESS; index_u16++)
    {
        blockContent_au8[index_u16 - offset_u16] = EeP_ReadByte(index_u16);
    }

    return blockContent_au8;
}

void EeP_EraseChip()
{
    for (uint16_t address_u16 = EEPROM_START_ADDRESS; address_u16 < EEPROM_END_ADDRESS; address_u16++)
    {
        EeP_WriteByte(address_u16, 0xFF);
    }
}
/* ------------------------- </IMPLEMENTATIONS> ------------------------- */

void setup()
{
    EeP_Init();
}

void loop()
{
    Utils_PrintMenu();

    while (Serial.available() == 0)
        ;

    int choice = Serial.read();
    Serial.print(choice - '0');

    uint16_t address_u16;
    switch (choice)
    {
    case '1':
        address_u16 = Utils_GetAddressFromSerial();

        Serial.print(Utils_AddressToHexString(address_u16));
        Serial.print(": ");
        Serial.println(Utils_ByteToHexString(EeP_ReadByte(address_u16)));
        break;

    case '2':
        address_u16 = Utils_GetAddressFromSerial();

        Serial.println(Utils_BlockToString(address_u16, EeP_ReadBlock(address_u16)));
        break;

    case '5':
        Serial.println();
        Serial.println("This will erase all data on the chip! Do you wish to continue? (y/n) ");
        while (Serial.available() == 0)
            ;

        if (Serial.read() == 'y')
            EeP_EraseChip();
        break;

    default:
        Serial.println("Invalid choice!");
        break;
    }
}