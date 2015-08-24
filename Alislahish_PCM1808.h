/**
*
* Alislahish_PCM1808.h
* by anwar hahj jefferson-george
*
* This file can be used to represent the functions and settings of the
*
*  PCM1808
*  SINGLE-ENDED, ANALOG-INPUT 24-BIT, 96-kHz STEREO A/D CONVERTER by Burr-Brown Products 
*  from Texas Instruments.
*
*  Product page:
*  http://www.ti.com/product/pcm1808
*  Datasheet (Rev. A):
*  http://www.ti.com/lit/gpn/pcm1808
**********************************************************************
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
*/

#include "Arduino.h"
#include "ICUsingMCP23017.h"

//only include this library description once
#ifndef ALISLAHISH_PCM1808_H
#define ALISLAHISH_PCM1808_H

///////////////some operational constants (see datasheet)
/*
to avoid pop noise, PCM1808 has fade in and fade out on DOUT(9)
level changes from 0 dB <-> mute using calculated pseudo S-shaped characteristics
with zero-cross detection, so the time needed depends on the analog input frequency
it takes 48 / fin, or if there's no zero cross during 8192/fs, 48/fs (timeout)
*/
#define FADE_TIME_OUT(fs) (48L/fs)

/*
halting SCKI(6) with a fixed high or low signal any time after startup
triggers PCM1808's power-down and reseat function. Reset and power down are then performed
automatically within a minimum 4us after SCKI halt. While clock-halt reset is asserted,
PCM1808 stays in reseet and power-down mode, forcing DOUT(9) to zero. SCKI must be 
resupplied to release reset and power-down mode; digital output becomes valid after 
reset state is released and time of 1024*SCKI + 8960/fs has elapsed. Due to fade-in
output may not correspond to analog input signal until an additional 48/fin or 48/fs
*/
#define SKCI_RESUME_TO_RESET_RELEASE_TIME(SCKI) (1024*SCKI)
#define RESET_RELEASE_TO_DOUT_OUTPUT_TIME(fs) (8960L/fs)


/*
After SCKI halted, BCK(8) & LRCK(7) must resync with SCKI
within 4480/fs after SCKI is resumed, therwise, SCKI should
be masked until synchronization is reattained to take care of
jitter and glitch
*/
#define CK_SYNC_TIME(fs) (4480L/fs) 
							
	

//supported sampling frequencies (fs)
enum class PCMSamplingFrequencies{
	HZ_8000 = 0x0,
	HZ_16000 = 0x1,	
	HZ_32000 = 0x2,
	HZ_44100 = 0x3,
	HZ_48000 = 0x4,
	HZ_68000 = 0x5,
	HZ_88200 = 0x6,
	HZ_96000 = 0x7
};

inline long getSamplingHz(PCMSamplingFrequencies freq){
	switch (freq){
		case PCMSamplingFrequencies::HZ_8000:
			return 8000L;
		case PCMSamplingFrequencies::HZ_16000:
			return 16000L;
		case PCMSamplingFrequencies::HZ_32000:
			return 32000L;
		case PCMSamplingFrequencies::HZ_44100:
			return 44100L;
		case PCMSamplingFrequencies::HZ_48000:
			return 48000L;
		case PCMSamplingFrequencies::HZ_68000:
			return 68000L;
		case PCMSamplingFrequencies::HZ_88200:
			return 88200L;
		case PCMSamplingFrequencies::HZ_96000:
			return 96000L;
	}
}

#define NUM_INTERFACE_MODES 4
//Supported interface modes
enum class InterfaceModes{
	SLAVE = 0x0, 		//Slave mode (256fs, 384fs, 512fs autodetect):	MD1 LOW, MD0 LOW
	MASTER_512 = 0x1, 	//Master mode (512 fs):							MD1 LOW, MD0 HIGH
	MASTER_384 = 0x2,	//Master mode (384 fs):							MD1 HIGH, MD0 LOW
	MASTER_256 = 0x3	//Master mode (256 fs):							MD1 HIGH, MD0 HIGH
};

#define NUM_AUDIO_INTERFACE_FORMATS 2
//Supported audio data formats 
enum class AudioInterfaceFormats{
	I2S = 0x0, //I^2S, 24-bit (FMT pin LOW)
	LEFT = 0x1 //Left-justified, 24-bit (FMT pin HIGH)
};

//library interface description
class Alislahish_PCM1808 : public ICUsingMCP23017 
{
	public:
		Alislahish_PCM1808();
		Alislahish_PCM1808(uint8_t FMTpin, uint8_t MD1pin, uint8_t MD0pin, uint8_t SCKIMASKpin,
						   uint8_t LRCKpin, uint8_t BCKpin, uint8_t DINpin);
		//selectSamplingFrequency(PCMSamplingFrequencies freq);
		void selectMode(InterfaceModes mode);
		void selectFormat(AudioInterfaceFormats format);
		InterfaceModes getMode();
		AudioInterfaceFormats getFormat();
		
		void begin();
		void begin(InterfaceModes mode, AudioInterfaceFormats format);

		void powerDownAndReset();
		void resume();

		bool isConverting();

	private:
		void setPinModes();
		void setPins();
		//operating characteristics
		//PCMSamplingFrequencies _fs;
		InterfaceModes _mode;
		AudioInterfaceFormats _format;
		bool _isConverting = false;

		//hardware connects - control
		uint8_t _FMTpin; // PCM1808 pin 12
		uint8_t _MD1pin; //	PCM1808 pin 11
		uint8_t _MD0pin; // PCM1808 pin 10

		/*
		connect this pin to the input of an AND gate
		whose companion input is connected to a clock generator
		*/
		uint8_t _SCKIMASKpin;
		//i2s hardware connects 
		uint8_t _LRCKpin; // PCM1808 pin 7
		uint8_t _BCKpin; // PCM1808 pin 8
		uint8_t _DOUTpin; // PCM1808 pin 9

};

#endif
