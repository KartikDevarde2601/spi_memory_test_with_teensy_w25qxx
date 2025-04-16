#include <SPI.h>

// Define the chip select pin
const int chipSelectPin = 10; // You can change this to the pin you connected CS to

void setup() {
  Serial.begin(115200);
  Serial.println("W25Qxx Flash Test");

  // Initialize SPI
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8); // Adjust clock speed if needed
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  // Configure the chip select pin as an output
  pinMode(chipSelectPin, OUTPUT);
  digitalWrite(chipSelectPin, HIGH); // Deselect the chip initially

  // Basic ID Read Test
  Serial.println("Reading Flash ID...");
  uint8_t manufacturerID = 0;
  uint8_t deviceID = 0;

  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0x90); // Read Manufacturer/Device ID command
  SPI.transfer(0x00); // Address bits (dummy for this command)
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  manufacturerID = SPI.transfer(0x00); // Read Manufacturer ID
  deviceID = SPI.transfer(0x00);      // Read Device ID
  digitalWrite(chipSelectPin, HIGH);

  Serial.print("Manufacturer ID: 0x");
  Serial.println(manufacturerID, HEX);
  Serial.print("Device ID: 0x");
  Serial.println(deviceID, HEX);

  // Basic Write and Read Test (Single Byte)
  Serial.println("\nBasic Write and Read Test...");
  uint32_t testAddress = 0x000000;
  uint8_t writeData = 0xA5;
  uint8_t readData = 0x00;

  // Enable Write Operations (assuming no hardware WP)
  enableWrite();

  // Write Data
  Serial.print("Writing 0x");
  Serial.print(writeData, HEX);
  Serial.print(" to address 0x");
  Serial.println(testAddress, HEX);
  writeByte(testAddress, writeData);

  // Read Data
  Serial.print("Reading from address 0x");
  Serial.println(testAddress, HEX);
  readData = readByte(testAddress);
  Serial.print("Read Data: 0x");
  Serial.println(readData, HEX);

  if (writeData == readData) {
    Serial.println("Write and Read test successful!");
  } else {
    Serial.println("Write and Read test failed!");
  }

  // Disable Write Operations (good practice)
  disableWrite();
}

void loop() {
  // put your main code here, to run repeatedly:
}

// Function to enable write operations (send Write Enable command)
void enableWrite() {
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0x06); // Write Enable command
  digitalWrite(chipSelectPin, HIGH);
  delay(1); // Small delay might be needed
}

// Function to disable write operations (send Write Disable command)
void disableWrite() {
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0x04); // Write Disable command
  digitalWrite(chipSelectPin, HIGH);
  delay(1); // Small delay might be needed
}

// Function to write a single byte to the flash memory
void writeByte(uint32_t address, uint8_t data) {
  // Send Page Program command (0x02)
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0x02);
  // Send 24-bit address
  SPI.transfer((address >> 16) & 0xFF);
  SPI.transfer((address >> 8) & 0xFF);
  SPI.transfer(address & 0xFF);
  // Send data byte
  SPI.transfer(data);
  digitalWrite(chipSelectPin, HIGH);

  // Wait for the write operation to complete (check status register or use delay)
  delay(5); // Adjust delay based on your flash chip's write cycle time
}

// Function to read a single byte from the flash memory
uint8_t readByte(uint32_t address) {
  uint8_t data;

  // Send Read Data command (0x03)
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(0x03);
  // Send 24-bit address
  SPI.transfer((address >> 16) & 0xFF);
  SPI.transfer((address >> 8) & 0xFF);
  SPI.transfer(address & 0xFF);
  // Read the data byte
  data = SPI.transfer(0x00);
  digitalWrite(chipSelectPin, HIGH);

  return data;
}