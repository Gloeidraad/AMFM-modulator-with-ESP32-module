#ifndef _MC44BS373CA_h_
#define _MC44BS373CA_h_

#include <stdint.h>
#include <wire.h>

#define MC44BS373CA_I2C_CLOCK_SPEED 1000000

class MC44BS373CA_Class {
  public:
     MC44BS373CA_Class(TwoWire &wire = Wire, uint32_t current_speed = 400000);
     void Init(int8_t check_if_present = 1, int8_t showdebug = 1 );
     uint8_t ChipPresent(void) { return _chip_present; }
     void NewChannel(int8_t ch,     int8_t showdebug   = 0) { Write(ch, showdebug); }
     void SetChannel(int8_t chan,   int8_t update_regs = 1) { _channel = chan;      if(update_regs) Write(); }
     void SetSystem(int8_t sys,     int8_t update_regs = 1) { _system = sys;        if(update_regs) Write(); }
     void SetSound(int8_t snd,      int8_t update_regs = 1) { _soundoff = !snd;     if(update_regs) Write(); }
     void SetRfOff(int8_t rfoff,    int8_t update_regs = 1) { _rfoff = rfoff;       if(update_regs) Write(); }
     void SetSubcarrier(int8_t sc,  int8_t update_regs = 1) { _subcarrier = sc & 3; if(update_regs) Write(); }
     void SetTestPattern(int8_t tp, int8_t update_regs = 1) { _testpattern = tp;    if(update_regs) Write(); }
     void ShowInfo(void);
     
  protected:
    uint16_t Read(void);
    void Write(uint16_t channel = 0, int8_t showdebug = 0);
    void SetWireClock(void)     { if(_prevspeed != MC44BS373CA_I2C_CLOCK_SPEED) _wire->setClock(MC44BS373CA_I2C_CLOCK_SPEED); }
    void RestoreWireClock(void) { if(_prevspeed != MC44BS373CA_I2C_CLOCK_SPEED) _wire->setClock(_prevspeed); }

    TwoWire * _wire;
    uint32_t _prevspeed;
    uint8_t  _chip_present;

    uint16_t _channel;
    uint8_t  _soundoff;      // 0 = sound on; 1 = sound off
    uint8_t  _rfoff;         // 0 = rf on; 1 = rf off
    uint8_t  _system;        // 0 = B/G; 1 = L 
    uint8_t  _subcarrier;    // 0 = 4.5; 1 = 5.5; 2 = 6.0; 3 = 6.5 MHz
    uint8_t  _testpattern;   // 0 = normal operationl; 1 = testpattern on
};

#endif
