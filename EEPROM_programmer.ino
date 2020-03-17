#define IO_0 10
#define IO_1 11
#define IO_2 12
#define IO_3 5
#define IO_4 6
#define IO_5 7
#define IO_6 8
#define IO_7 9

#define SER 2
#define SERCLK 3
#define RCLK 4
#define bWE 13

uint8_t hex_char_to_int(uint8_t c)
{
  if (c >= '0' && c <= '9')
    c -= '0';
  else if (c >= 'a' && c <= 'f')
    c = c - 'a' + 10;
  else if (c >= 'A' && c <= 'f')
    c = c - 'A' + 10;

  return c;
}

int32_t read_hex_address()
{
  int32_t address = 0;
  uint32_t buf[4];

  while (Serial.available() != 7);

  Serial.read();
  Serial.read();
  buf[0] = hex_char_to_int(Serial.read());
  buf[1] = hex_char_to_int(Serial.read());
  buf[2] = hex_char_to_int(Serial.read());
  buf[3] = hex_char_to_int(Serial.read());
  Serial.read();

  address += buf[0] * 16 * 16 * 16;
  address += buf[1] * 16 * 16;
  address += buf[2] * 16;
  address += buf[3];

  if (address > 0x7FFF)
  {
    Serial.println("Invalid address! (0x0000 - 0x7FFF)");
    return -1;
  }

  return address;
}

int32_t read_hex_value()
{
  int32_t value = 0;
  uint32_t buf[2];

  while (Serial.available() != 5);

  Serial.read();
  Serial.read();
  buf[0] = hex_char_to_int(Serial.read());
  buf[1] = hex_char_to_int(Serial.read());
  Serial.read();

  value += buf[0] * 16;
  value += buf[1];

  return value;
}

void print_menu()
{
  Serial.println("1. read eeprom");
  Serial.println("2. erase eeprom");
  Serial.println("3. read specific address");
  Serial.println("4. write to specific address");
}

void sendBit(uint8_t value)
{
  digitalWrite(SER, value);
  digitalWrite(SERCLK, HIGH);
  digitalWrite(SERCLK, LOW);
}

void setAddress(uint16_t address, uint8_t bOE)
{
  digitalWrite(SERCLK, LOW);

  sendBit(bOE);
  sendBit((address >> 0) & 1);
  sendBit((address >> 1) & 1);
  sendBit((address >> 2) & 1);
  sendBit((address >> 3) & 1);
  sendBit((address >> 4) & 1);
  sendBit((address >> 5) & 1);
  sendBit((address >> 6) & 1);
  sendBit((address >> 7) & 1);
  sendBit((address >> 8) & 1);
  sendBit((address >> 9) & 1);
  sendBit((address >> 10) & 1);
  sendBit((address >> 11) & 1);
  sendBit((address >> 12) & 1);
  sendBit((address >> 13) & 1);
  sendBit((address >> 14) & 1);

  digitalWrite(RCLK, LOW);
  digitalWrite(RCLK, HIGH);
  digitalWrite(RCLK, LOW);
}

byte readAddress(uint16_t address)
{
  for (uint8_t pin = 5; pin <= 12; pin++)
    pinMode(pin, INPUT);

  setAddress(address, LOW);

  byte data = 0;
  data = digitalRead(IO_7);
  data <<= 1;
  data |= digitalRead(IO_6);
  data <<= 1;
  data |= digitalRead(IO_5);
  data <<= 1;
  data |= digitalRead(IO_4);
  data <<= 1;
  data |= digitalRead(IO_3);
  data <<= 1;
  data |= digitalRead(IO_2);
  data <<= 1;
  data |= digitalRead(IO_1);
  data <<= 1;
  data |= digitalRead(IO_0);

  return data;
}

void readEEPROM(uint16_t beg_addr, uint16_t end_addr)
{
  byte data[16];
  char buf[128];

  while (beg_addr % 16 != 0)
    beg_addr--;

  for (uint16_t base = beg_addr; base <= end_addr; base += 16)
  {
    for (uint8_t offset = 0; offset <= 15; offset++)
      data[offset] = readAddress(base + offset);

    sprintf(buf, "%04X:  %02X %02X %02X %02X %02X %02X %02X %02X   %02X %02X %02X %02X %02X %02X %02X %02X",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}

void writeAddress(uint16_t address, uint8_t value)
{
  setAddress(0x0000, HIGH);
  for (uint8_t pin = 5; pin <= 12; pin++)
    pinMode(pin, OUTPUT);

  setAddress(address, HIGH);

  digitalWrite(bWE, LOW);

  digitalWrite(IO_0, (value >> 0) & 1);
  digitalWrite(IO_1, (value >> 1) & 1);
  digitalWrite(IO_2, (value >> 2) & 1);
  digitalWrite(IO_3, (value >> 3) & 1);
  digitalWrite(IO_4, (value >> 4) & 1);
  digitalWrite(IO_5, (value >> 5) & 1);
  digitalWrite(IO_6, (value >> 6) & 1);
  digitalWrite(IO_7, (value >> 7) & 1);

  digitalWrite(bWE, HIGH);

  delay(3);
}

void eraseEEPROM()
{
  Serial.println("Erasing EEPROM...");

  for (uint32_t address = 0; address <= 0x7FFF; address++)
  {
    if (address == 0x1FFF)
      Serial.println("25%");
    else if (address == 0x3FFF)
      Serial.println("50%");
    else if (address == 0x5FFF)
      Serial.println("75%");
    writeAddress(address, 0xFF);
  }
  Serial.println("Erasing done!");
}

void setup() {
  pinMode(SER, OUTPUT);
  pinMode(SERCLK, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(bWE, OUTPUT);

  digitalWrite(bWE, HIGH);

  Serial.begin(57600);
}

void loop() {
  Serial.println();
  print_menu();

  uint8_t choice;
  while(Serial.available() == 0);
  choice = Serial.read();
  
  int32_t addr;
  switch (choice)
  {
    case '1':
      Serial.println("Beginning address (0x----): ");
      int32_t addr1;
      while ((addr1 = read_hex_address()) == -1);
      
      Serial.println("End address (0x----): ");
      int32_t addr2;
      while ((addr2 = read_hex_address()) == -1);
      
      readEEPROM(addr1, addr2);
      break;
    case '2':
      Serial.println("This will erase the entire EEPROM! Are you sure? (y/n)");
      while(Serial.available() == 0);
      if(Serial.read() == 'y')
        eraseEEPROM();
      break;
    case '3':
      Serial.println("Address (0x----): ");
      while ((addr = read_hex_address()) == -1);
      
      Serial.print("The value at address ");
      Serial.print(addr, HEX);
      Serial.print(" is: ");
      Serial.println(readAddress(addr), HEX);
      break;
    case '4':
      Serial.println("Address (0x----): ");
      while ((addr = read_hex_address()) == -1);
      
      int32_t value = read_hex_value();
      writeAddress(addr, value);
      
      Serial.print("Wrote ");
      Serial.print(value, HEX);
      Serial.print(" to address ");
      Serial.print(addr, HEX);
      Serial.println();
      break;
    default:
      Serial.println(choice);
      break;
  }
  
}
