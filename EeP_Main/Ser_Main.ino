#include "Ser_Main.h"

void Ser_Init()
{
    Serial.begin(9600);

    pinMode(SERIAL_DATA_PIN, OUTPUT);
    pinMode(SERIAL_RCLK_PIN, OUTPUT);
    pinMode(SERIAL_SCLK_PIN, OUTPUT);

    digitalWrite(SERIAL_DATA_PIN, LOW);
    digitalWrite(SERIAL_RCLK_PIN, LOW);
    digitalWrite(SERIAL_SCLK_PIN, LOW);
}

void Ser_SendSerialBit(uint8_t bit_u8)
{
    digitalWrite(SERIAL_DATA_PIN, bit_u8);
    digitalWrite(SERIAL_SCLK_PIN, HIGH);
    digitalWrite(SERIAL_SCLK_PIN, LOW);
}

void Ser_SendSerialByte(uint8_t byte_u8)
{
    digitalWrite(SERIAL_SCLK_PIN, LOW);

    for (uint8_t index_u8 = 0; index_u8 < 8; index_u8++)
    {
        Ser_SendSerialBit((byte_u8 >> index_u8) & 0x01);
    }
}

void Ser_SendSerialWord(uint16_t word_u16)
{
    Ser_SendSerialByte((uint8_t)(word_u16 & 0x00FF));
    Ser_SendSerialByte((uint8_t)((word_u16 >> 8) & 0x00FF));

    digitalWrite(SERIAL_RCLK_PIN, LOW);
    digitalWrite(SERIAL_RCLK_PIN, HIGH);
    digitalWrite(SERIAL_RCLK_PIN, LOW);
}