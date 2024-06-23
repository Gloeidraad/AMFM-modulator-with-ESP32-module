//************************************************************************
// Driver for UHF TV Transmitter MC44BS373CA
//************************************************************************

#include <arduino.h>
#include "ESP32CompositeVideo.h"

ESP32CompositeVideo_Class::ESP32CompositeVideo_Class(TwoWire &wire, uint32_t current_speed) {
  _prevspeed    = current_speed;
  _wire         = &wire;
  _chip_present = false;
  _nr_images    = 0;
}

void ESP32CompositeVideo_Class::Init(uint8_t show_info) {
  SetWireClock();
  _wire->beginTransmission(ESP32_COMP_VIDEO_I2C_ADDRESS);
  _chip_present = _wire->endTransmission() == 0;
  RestoreWireClock();
  if(_chip_present) {
    _nr_images = 0;
    for(int i = 0; i < 5; i++) { // Send multiple times to kick off
      SendCommand("IC");
      delay(5);
    }
  }
  if(show_info)
    ShowInfo();
}

void ESP32CompositeVideo_Class::ShowInfo(void) {
  Serial.print("ESP32 Composite Video Module ");
  if(!_chip_present) Serial.print("not ");
  Serial.print("present!");
  if(_chip_present) {
    Serial.print(" (Containing ");
    Serial.print((int)_nr_images);
    Serial.print(" images)");
  }
  Serial.println();
}

void ESP32CompositeVideo_Class::SendCommand(const char * cmd, const char * param) {
  if(_chip_present) {
    SetWireClock();
    _wire->flush();
    _wire->beginTransmission(ESP32_COMP_VIDEO_I2C_ADDRESS);
    _wire->write(cmd);
    if(param != NULL)
      _wire->write(param);
    _wire->write('\0');
    _wire->endTransmission();
    if(_wire->requestFrom(ESP32_COMP_VIDEO_I2C_ADDRESS, (uint8_t)1) == 1) {
      _nr_images = _wire->read(); // Reading returns always _nr_images
    }
    RestoreWireClock();
  }
}

void ESP32CompositeVideo_Class::SetTextLineA(const char *s, uint8_t append) {
  SendCommand(append ? "AA" : "LA", s);
}

void ESP32CompositeVideo_Class::SetTextLineB(const char *s, uint8_t append) {
  SendCommand(append ? "AB" : "LB", s);
}

void ESP32CompositeVideo_Class::ShowImage(uint8_t image) {
  if( image < _nr_images) {
    SendCommand("SI", String(image).c_str());
  }
}
