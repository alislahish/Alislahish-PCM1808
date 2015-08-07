/**
* This example tests the Alislahish-PCM1808 library using a PLL1705 multiclock generator
* to create a valid sampling clock to signal a PCM1808 24-bit stereo ADC.
* 
* * If you set it up correctly, you'll see the digital outputs  
* (on PCM1808 pins 7-9) start for a period, then the builtin LED will turn on and the outputs
* should go dead. This behavior cycles indefinitely.
* 
* Dependencies:
*  Alislahish-ICUsingMCP23017 library
*  Alislahish-PCM1705 library
* 
* 2015 by Anwar Hahj Jefferson-George
* 
* Look in the folder "examples/digitaloutput" that 
* is included with this library to see images of 
* how valid digital outputs on PCM1808 pins 7, 8, 9 
* should appear
* 
* In this example the pin connections are as follows 
*    ESP8266    ->    MCP23017
*    GPIO 4(SDA)->    Pin 13 
*    GPIO 5(SCL)->    Pin 12
*    
*    MCP23017
*    Pin 9 -> 3.3V, Pin 10 -> GND, 
*    Pin 15, 16, 17 (A0, A1, A2) -> GND, Pin 18 (!RESET) -> Vcc via 10K resistor 
*    
*     MCP23017        ->    PLL1705
*    Pin 1 (GBP0)    ->    Pin 5 (FS1) 
*    Pin 2 (GPB1)    ->    Pin 6 (FS2)
*    Pin 3 (GBP2)    ->    Pin 7 (SR)
*    Pin 4 (GBP3)    ->    Pin 12 (CSEL)  
*    You can also connect PLL1705 CSEL to ground or high 
*    if you don't need the MCKO1 or MCKO2 output control  
*
*    PLL1705
*    Connect a 27 MHz crystal across pins 10 & 11 (XT1 & XT2), 
*    then add two equal-valued  capacitors from pins 10 and 11 to ground to create a pi network.
*    Consult your crystal datasheet for load capacitance and calculate appropriate pi network values:
*    See: http://www.mecxtal.com/pdf/te_notes/tn-021.pdf
*
*    Bridge the following pin pairs with individual 0.1 uF ceramic capacitors:
*            pins 1 & 4 (Vdd1 & DGND1),
*            pins 8 & 9 (Vcc & AGND), 
*            pins 13 & 16 (Vdd2 & DGND2), and
*            pins 17 & 20 (DGND3 & Vdd3)
*
*    Bridge the following pin pairs with individual 10 uF aluminum electrolytic capactiors:
*            pins 8 & 9 (Vcc & AGND), 
*            pins 1 & 4 (Vdd & DGND1) 
*
*    Connect Pins 1, 8 (Vdd, Vcc) to 3.3V supply. 
*    Connect pins 4, 9, 16, 17 (DGND1, AGND, DGND2, DGND3) to ground. 
*    
*    MCP23017           -> PCM1808
*    Pin 5 (GPB4)       -> Pin 12 (FMT)
*    Pin 6 (GPB5)       -> Pin 11 (MD1)
*    Pin 7 (GPB6)       -> Pin 10 (MD0)
*    
*    CLOCK CONTROL
*    Using an and gate, you can disable the clock signal from the PLL1705.
*    This is unnecessary if you generate a clock signal from your microcontroller,
*    but in this setup, I used an MC74LVX08 as follows:
*    
*    MCP23017 Pin 8 (GPB7)      -> AND GATE Input A  
*    PLL1705 Pin 2 (SCKO2 256fs)-> AND GATE Input B
*    
*    AND GATE Output            -> PCM1808 Pin 6 (SCKI)
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <ICUsingMCP23017.h>
#include <Alislahish_PCM1808.h>
#include <Alislahish_PLL1705.h>
#include <i2s.h>

//PLL1705 - MCP23017
#define FS1_PIN 8 //GPB0 - pin 1
#define FS2_PIN 9 //GPB1 - pin 2
#define SR_PIN 10 //GPB2 - pin 3
#define CSEL_PIN 11 //GPB3 -pin 4

//PCM1808
    //I2S - direct ESP8266 GPIO connections
#define DOUT_PIN 3
#define BCK_PIN 15
#define WS_PIN 2
    //CONTROL - MCP23017
#define FMT_PIN 12 //GPB4 - pin 5
#define MD1_PIN 13 //GPB5 - pin 6
#define MD0_PIN 14 //GPB6 - pin 7
    //MC74LVX08 AND gate control 
#define SCLK_ENABLE_PIN 15//GPB7 - pin 8

Alislahish_PLL1705 pll(CSEL_PIN, FS1_PIN, FS2_PIN, SR_PIN);
Alislahish_PCM1808 pcm(FMT_PIN, MD1_PIN, MD0_PIN, SCLK_ENABLE_PIN, WS_PIN, BCK_PIN, DOUT_PIN);

Adafruit_MCP23017 mcp;
uint8_t mcpAddr = 0;
long lastMillis = 0L;
PLLSamplingFrequencies fs = PLLSamplingFrequencies::HZ_32000;
bool pcmOn = false;
uint8_t led = HIGH;

#define ON_TIME 4181

void setup() {
    Serial.begin(115200);
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    //start MCP23017
    mcp.begin(mcpAddr);
    //make the PLL1705 use the MCP23017
    pll.setMCP23017(true, &mcp, mcpAddr);
    //make the PCM1808 use the MCP23017
    pcm.setMCP23017(true, &mcp, mcpAddr);
    //start pll1705
    pll.begin(fs);
    //start pcm1808
    pcmOn = true;
    pcm.begin(); //Default to Master 256fs, I2S audio
}


void loop() {
    //turn on and off the PCM1808 ADC
    if( millis() >= (lastMillis + ON_TIME) ){
        lastMillis = millis();
        if(pcmOn){
            pcm.powerDownAndReset();
            led=LOW;
        } else {
            pcm.resume();
            led=HIGH;
        }
        pcmOn = !pcmOn;
        digitalWrite(BUILTIN_LED, led);
    }

}
