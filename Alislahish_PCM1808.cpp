/**********************************************************************
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**********************************************************************/
#include "Alislahish_PCM1808.h"

/**
* constructor using pins
*/
Alislahish_PCM1808::Alislahish_PCM1808(uint8_t FMTpin, uint8_t MD1pin, uint8_t MD0pin, uint8_t SCKIMASKpin,
						   uint8_t LRCKpin, uint8_t BCKpin, uint8_t DOUTpin)
	:	_FMTpin(FMTpin), 
		_MD1pin(MD1pin),
		_MD0pin(MD0pin),
		_SCKIMASKpin(SCKIMASKpin),
		_LRCKpin(LRCKpin),
		_BCKpin(BCKpin),
		_DOUTpin(DOUTpin)
{
}

/**
* use appropriate hardware pins to choose a sampling frequency
*/
// Alislahish_PCM1808::selectSamplingFrequency(PCMSamplingFrequencies freq){}

/**
* use to select the interface mode...
* when adding slave PCM1808s with an existing master,
* conserve pins by hardwiring MD1 and MD2, or connecting multiple
* PCM1808s to the same bus
*/
void Alislahish_PCM1808::selectMode(InterfaceModes mode){
	_mode = mode;
	setPins();
}

/**
* use to choose the audio data output format
*/
void Alislahish_PCM1808::selectFormat(AudioInterfaceFormats format){
	_format = format;
	setPins();

} 


/**
* begin sampling at a default (Master 256) interface mode and default I2S data format
*/
void Alislahish_PCM1808::begin(){
	begin(InterfaceModes::MASTER_256, AudioInterfaceFormats::I2S);	
}

/**
* begin sampling at given interface mode
*/
void Alislahish_PCM1808::begin(InterfaceModes mode, AudioInterfaceFormats format){
	setPinModes();
	//selectSamplingFrequency(freq);
	selectMode(mode);
	selectFormat(format);
	resume();
}


/**
* halt the SCKI signal to initiate power down and reset
*/
void Alislahish_PCM1808::powerDownAndReset(){
	ICUsingMCP23017::digitalWrite(_SCKIMASKpin, LOW);

}

/**
* resume the SCKI signal
*/
void Alislahish_PCM1808::resume(){
	ICUsingMCP23017::digitalWrite(_SCKIMASKpin, HIGH);
}


/**
* set hardware pin modes
*/
void Alislahish_PCM1808::setPinModes(){
	ICUsingMCP23017::pinMode(_FMTpin, OUTPUT);
	ICUsingMCP23017::pinMode(_MD1pin, OUTPUT);
	ICUsingMCP23017::pinMode(_MD0pin, OUTPUT);
	ICUsingMCP23017::pinMode(_SCKIMASKpin, OUTPUT);
	ICUsingMCP23017::pinMode(_LRCKpin, OUTPUT);
	ICUsingMCP23017::pinMode(_BCKpin, OUTPUT);
	ICUsingMCP23017::pinMode(_DOUTpin, OUTPUT);
}

/**
* set hardware pins to current operating mode
*/
void Alislahish_PCM1808::setPins(){
	//interface mode
	switch(_mode){
		case InterfaceModes::SLAVE:
			ICUsingMCP23017::digitalWrite(_MD1pin, LOW);
			ICUsingMCP23017::digitalWrite(_MD0pin, LOW);
			break;
		case InterfaceModes::MASTER_512:
			ICUsingMCP23017::digitalWrite(_MD1pin, LOW);
			ICUsingMCP23017::digitalWrite(_MD0pin, HIGH);
			break;
		case InterfaceModes::MASTER_384:
			ICUsingMCP23017::digitalWrite(_MD1pin, HIGH);
			ICUsingMCP23017::digitalWrite(_MD0pin, LOW);
			break;
		case InterfaceModes::MASTER_256:
			ICUsingMCP23017::digitalWrite(_MD1pin, HIGH);
			ICUsingMCP23017::digitalWrite(_MD0pin, HIGH);
			break;				
	}

	//audio data format
	switch(_format){
		case AudioInterfaceFormats::I2S:
			ICUsingMCP23017::digitalWrite(_FMTpin, LOW);
			break;
		case AudioInterfaceFormats::LEFT:
			ICUsingMCP23017::digitalWrite(_FMTpin, HIGH);
			break;
	}
}