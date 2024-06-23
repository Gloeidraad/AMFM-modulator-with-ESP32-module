//************************************************************************
// Driver for UHF TV Transmitter MC44BS373CA
//************************************************************************

#include <arduino.h>
#include "MC44BS373CA.h"

#define MC44BS373CA_ADDRESS           0x65

#define MC44BS373CA_VHF_CHAN_MIN         2
#define MC44BS373CA_VHF_CHAN_MAX        12
#define MC44BS373CA_VHFI_CHAN_MAX        4
#define MC44BS373CA_VHFII_CHAN_MIN       5
#define MC44BS373CA_VHFI_FREQ_MIN     4825  // in kHz * 10
#define MC44BS373CA_VHFII_FREQ_MIN   17525  // in kHz * 10
#define MC44BS373CA_VHF_FREQ_STEP      700  // in kHz * 10

#define MC44BS373CA_UHF_CHAN_MIN        21
#define MC44BS373CA_UHF_CHAN_MAX        69
#define MC44BS373CA_UHF_FREQ_MIN     47125  // in kHz * 10
#define MC44BS373CA_UHF_FREQ_STEP      800  // in kHz * 10

#define MC44BS373CA_DEFAULT_CHAN        36
#define MC44BS373CA_SOUNDMASK_ON      0x80  // C1 bits 7,6,5,4,3,0
#define MC44BS373CA_SOUNDMASK_OFF     0xA0  // C1 bits 7,6,5,4,3,0

MC44BS373CA_Class::MC44BS373CA_Class(TwoWire &wire, uint32_t current_speed) {
  _prevspeed    = current_speed;
  _wire         = &wire;
  _chip_present = false;
  _channel      = MC44BS373CA_DEFAULT_CHAN;
  _soundoff     = false; // sound on
  _rfoff        = true;  // default is off
  _system       = 0;  // sytem B/G 
  _subcarrier   = 1;  // 5.5 MHz
  _testpattern  = 0;  // normal operation
}

void MC44BS373CA_Class::Init(int8_t check_if_present, int8_t showdebug) {
  int count = 0;
  if(check_if_present) {
    SetWireClock();
    _wire->beginTransmission(MC44BS373CA_ADDRESS);
    _chip_present = _wire->endTransmission() == 0;
    RestoreWireClock();
    if(showdebug) {
      Serial.print("MC44BS373CA ");
      if(!_chip_present) Serial.print("not ");
      Serial.println("found!");
    }
  }
  else
    _chip_present = 1; // Assume chip is always present
  if(_chip_present)
    Write();
}

/*
ch   video    audio
 2   48.25    53.75  \
 3   55.25    60.75   |
 4   62.25    67.75   | end of VHFI
 5  175.25   180.75   | start of VHFIII
 6  182.25   187.75   |
 7  189.25   194.75    > Steps 7 MHz
 8  196.25   201.75   |
 9  203.25   208.75   |
10  210.25   215.75   |
11  217.25   222.75   |
12  224.25   229.75  /

Ch  Video   Audio
21  471.25  476.75  \
22  479.25  484.75   |
.                    |
.                     > Steps of 8 MhZ 
.                    |
68  847.25  852.75   |
69  855.25  860.75  /
*/

void MC44BS373CA_Class::Write(uint16_t ch, int8_t showdebug) {
  if(ch >= 2  && ch <= 12) _channel = ch;
  if(ch >= 21 && ch <= 69) _channel = ch;

  if(_chip_present) {
    unsigned N,C1,C0,FM,FL;;
    C1 = _soundoff ? MC44BS373CA_SOUNDMASK_OFF : MC44BS373CA_SOUNDMASK_ON;
    FL = 0; // Set X1X0 to 0 (no div)
    if(_channel >= MC44BS373CA_UHF_CHAN_MIN) {
      N = (MC44BS373CA_UHF_FREQ_MIN + (_channel - MC44BS373CA_UHF_CHAN_MIN) * MC44BS373CA_UHF_FREQ_STEP) * 4;
    }
    else if(_channel >= MC44BS373CA_VHFII_CHAN_MIN) {
      FL = 2; // Set X1X0 to 2 (div 4)
      N  = (MC44BS373CA_VHFII_FREQ_MIN + (_channel - MC44BS373CA_VHFII_CHAN_MIN) * MC44BS373CA_VHF_FREQ_STEP) * 4*4;
    }
    else if(_channel == MC44BS373CA_VHFI_CHAN_MAX) {
      FL = 3; // Set X1X0 to 3 (div 8)
      N  = (MC44BS373CA_VHFI_FREQ_MIN + (MC44BS373CA_VHFI_CHAN_MAX - MC44BS373CA_VHF_CHAN_MIN) * MC44BS373CA_VHF_FREQ_STEP) * 4*8;
    }
    else {
      C1 |= 2; // Set X2 bit (div 16)
      N = (MC44BS373CA_VHFI_FREQ_MIN + (_channel - MC44BS373CA_VHF_CHAN_MIN) * MC44BS373CA_VHF_FREQ_STEP) * 4*16;
    }
    N  /= 100;
    FM  = N >> 6;
    FL |= (N & 0x3F) << 2;
    C0 = (_subcarrier & 3) << 3;
    if(_system) {
      C1 |= 0x01; // set SYSL bit
      C0 |= 0x80; // set PWC bit
    }
    if(_testpattern) {
      FM |= 0x40; // set TPEN bit
    }
    if(_rfoff) {
     //The MC44BS373CA can be set to standby mode with a combination of 3 bits: OSC = 1, SO = 1, and ATT = 1.
     C1 |= 0x20; // SO  = 1
     C0 |= 0x60; // OSC = 1, ATT = 1
     if(showdebug)
       Serial.println("MC44BS373CA turned off");
    }
    else if(showdebug)
      Serial.printf("MC44BS373CA CHAN=%2d => N=%d = %d kHz (regs: C1=0x%02X, C0=0x%02X, FM=0x%02X, FL=0x%02X)\n",_channel,N, N*250, C1,C0,FM,FL);
    SetWireClock();
    _wire->beginTransmission(MC44BS373CA_ADDRESS);
    _wire->write(C1);
    _wire->write(C0);
    _wire->write(FM);
    _wire->write(FL);
    _wire->endTransmission();
    RestoreWireClock();
  }
}

uint16_t MC44BS373CA_Class::Read(void) {
  if(_chip_present) {
    uint8_t val = 0, ok;
    SetWireClock();
    _wire->requestFrom(MC44BS373CA_ADDRESS, 1);
    ok = _wire->readBytes(&val, 1) == 1;
    RestoreWireClock();
    if(ok)
      return val;
  }
  return -1;
}

void MC44BS373CA_Class::ShowInfo(void) {
  Serial.println("MC44BS373CA info");
  if(_chip_present) {
    Serial.printf("  Channel    : %d\n", _channel);
    Serial.printf("  Sound      : %s\n", _soundoff ? "Off" : "On");
    Serial.printf("  System     : %s audio\n", _system  ? "Positive Video, AM" : "Negative Video, FM");
    Serial.print ("  Subcarrier : ");
    switch( _subcarrier) {
      case 0 : Serial.print("4.5"); break;
      case 1 : Serial.print("5.5"); break;
      case 2 : Serial.print("6.0"); break;
      default: Serial.print("6.5"); break;
    }
    Serial.println(" MHz");
    Serial.printf("  Testpattern: %s\n", _testpattern ? "On" : "Off");
  }
  else
    Serial.println("  Device not present!");
}
