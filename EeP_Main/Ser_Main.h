#ifndef SER_MAIN_H
#define SER_MAIN_H

#define SERIAL_DATA_PIN 2
#define SERIAL_RCLK_PIN 3
#define SERIAL_SCLK_PIN 4

#define SERIAL_BAUD_RATE 115200

void Ser_Init();
void Ser_SendSerialBit(uint8_t bit_u8);
void Ser_SendSerialByte(uint8_t byte_u8);
void Ser_SendSerialWord(uint16_t word_u16);

#endif