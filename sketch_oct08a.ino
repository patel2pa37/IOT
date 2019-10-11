#include <SPI.h>

//use pin #7 as CS
const int CS_PIN = 7;

byte readRegister(byte address)
{
  address |= 0x80; //set MSB high for READ
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(address);
  byte rx = SPI.transfer(0x0);
  digitalWrite(CS_PIN, HIGH);
  return rx;
}

void writeRegister(byte address, byte value)
{
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(CS_PIN, HIGH);

  return;
}

void setup() {

  //setup SPI interface for CPOL=1, CPHA=1 or SPI_MODE_3 in Arduino
  SPI.begin();
  SPI.setDataMode(SPI_MODE3); //this works best. New SPI_TRANSACTION is not well documented

  //set CS pin to HIGH
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN,HIGH);
}

void loop() {

  //Step #1: Attempt to read from DEVICE ID REGISTER
  //Serial.println(readRegister(0x00)); 
  //Step #2: Check to see if value returned was correct
 
  //Step #3: Attempt to write to known register (try TAP_THRESH as we don't use that feature)
  //(writeRegister(0x1D,0x1D));
  Serial.println(readRegister(0x00));
  //Step #4: Read back your written value from TAP_THRESH. It should be the same.
}
