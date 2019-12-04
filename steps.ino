#include <SPI.h>
#define Xbee Serial1

int csPin=7;

void setupADXL(int _CS_PIN)
{
  
  //***Step #0***
  csPin=_CS_PIN;
  
  //***Step #1***
  //call SPI.begin() to initialize the SPI interface
  SPI.begin();
  Xbee.begin(9600);
  
  //***Step #2***
  //use SPI.setDataMode() to select the appropriate SPI mode for operations. 
  //See: https://www.arduino.cc/en/Reference/SPISetDataMode and the lecture notes
  SPI.setDataMode(SPI_MODE3);
  Serial.begin(9600);

  //***Step #3 (Optional) ***
  //write to the DATA_FORMAT register to adjust the sampling range
  
  //***Step #4 (Optional) ***
  //write to the DATA_FORMAT register to adjust the sampling range
  
  //***Step #5***
  //write to the MEASURE bit in the POWER_CLT register
  //to begin sampling. Ensure the write does not modify other bits by
  //reading the current value, modifying the data locally, and then writing
  //back the new register value.
   pinMode(csPin, OUTPUT);
  digitalWrite(csPin,HIGH);
  writeRegister(0x31, 0x00);//4g
  writeRegister(0x2D, 0x08);
  
}

void writeRegister(uint8_t address, uint8_t value)
{
  //bring CS pin LOW to begin communication
  digitalWrite(csPin, LOW);
  
  //send address of register to be written
  SPI.transfer(address);
  
  //send new value to be written to the register
  SPI.transfer(value);
  
  //bring CS pin HIGH to end communication
  digitalWrite(csPin, HIGH);

  return;
}

byte readRegister(uint8_t address)
{
  //modify the address to set bit 7 HIGH to indicate a read
  address |= 0x80; //set MSB high for READ
  
  //bring the CS pin low to begin communication
  digitalWrite(csPin, LOW);
  
  //transfer the register address
  SPI.transfer(address);
  
  //transfer bogus data so the accelerometer can respond
  uint8_t rx = SPI.transfer(0x0);
  
  //bring CS pin HIGH to end communication
  digitalWrite(csPin, HIGH);
  
  //return the data read from the register
  return rx;
}

bool selfTest()
{
  
  //implement code to check if you can
  //read the DEVICE_ID register. Return TRUE if
  //the correct result is found, otherwise FALSE.
  uint8_t device_id = readRegister(0x00);
  if (device_id==229){
  return true;}
  else{
  return false;}
}

void readMultiBytes(uint8_t baseAddress, int num, uint8_t data[])
{
  
  return;
}

int16_t getXAcceleteration()
{
  uint8_t x1 = readRegister(0x32);
  uint8_t x2 = readRegister(0x33);
  uint16_t xTotal = (x2<<8)|x1; 
  return xTotal;
}

int16_t getYAcceleteration()
{  
  uint8_t y1 = readRegister(0x34);
  uint8_t y2 = readRegister(0x35);
  uint16_t yTotal = (y2<<8)|y1; 
  return yTotal;
}

int16_t getZAcceleteration()
{
  uint8_t z1 = readRegister(0x36);
  uint8_t z2 = readRegister(0x37);
  uint16_t zTotal = (z2<<8)|z1; 
  return zTotal;
}

float convertRawToFloat(int16_t value)
{
 float Gs;
 Gs = float(value)/128; 
 return Gs;
}


void setup() {

  //**Step #1: Initialize the ADXL345 and select your CS pin**
  setupADXL(7);
  
  //**Step #2: Perform a self-test to see if you can read
  //from the accelerometer. Device will not progress is self-test fails.
  bool success = selfTest();
  
  //infinite loop if self-test fails
  while(!success){
    success = selfTest();
    }
}

void loop() {
  
  //Serial.println(getXAcceleteration());
  //read x-axis
  int16_t xAxis=getXAcceleteration();
  
  //read y-axis
  int16_t yAxis=getYAcceleteration();
  
  //read z-axis
  int16_t zAxis=getZAcceleteration();

  //Serial.print(xAxis); Serial.print(",");Serial.print(yAxis); Serial.print(",");Serial.println(zAxis);
  Serial.print(xAxis, DEC);
  Serial.print(',');
  Serial.print(yAxis, DEC);
  Serial.print(',');
  Serial.println(zAxis, DEC); 
  
  int SQRTvalues = (sqrt(xAxis*xAxis+yAxis*yAxis+zAxis*zAxis));
  //Serial.println(SQRTvalues);
  String str = (String(xAxis)+"//"+String(yAxis)+"//"+String(zAxis));
  //Xbee.println(xAxis);
  //Xbee.println(yAxis);
  Xbee.println(str);
  delay(100);
  //your Arduino will hate this but we need to test....
  //int magnitude = sqrt(xAxis*xAxis + yAxis*yAxis + zAxis*zAxis);
  //Serial.println(magnitude);
  //result should be ~1g when ADXL345 is not moving
  //float acceleration = convertRawToFloat(magnitude);
  //Serial.println(acceleration);
}
