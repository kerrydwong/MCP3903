#include "MCP3903.h"
#include <SPI.h>

MCP3903 mcp3903;

void setup()
{
    SPI.begin();
    Serial.begin(9600);
    
    mcp3903.reset(MCP3903::OSR_256);    
    mcp3903.setGain(1,MCP3903::GAIN_8);
}

void loop()
{
    Serial.println(mcp3903.readADC(1) * 2.36 , 4);
    delay(100);
}
