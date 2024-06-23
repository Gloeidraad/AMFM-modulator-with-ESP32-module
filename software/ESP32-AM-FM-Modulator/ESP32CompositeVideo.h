#ifndef _ESP32COMPOSITEVIDEO_H_
#define _ESP32COMPOSITEVIDEO_H_

#include <stdint.h>
#include <wire.h>

#define ESP32_COMP_VIDEO_I2C_CLOCK_SPEED     100000
#define ESP32_COMP_VIDEO_I2C_ADDRESS  (uint8_t)0x3D 

#define ESP32_COMP_VIDEO_IMAGE_RMA_1946   0
#define ESP32_COMP_VIDEO_IMAGE_FUBK_T1A   1
#define ESP32_COMP_VIDEO_IMAGE_FUBK_T1B   2
#define ESP32_COMP_VIDEO_IMAGE_ARIB       3
#define ESP32_COMP_VIDEO_IMAGE_FUBK_T2A   4
#define ESP32_COMP_VIDEO_IMAGE_FUBK T2B   5
#define ESP32_COMP_VIDEO_IMAGE_PM5540     6
#define ESP32_COMP_VIDEO_IMAGE_PM5544     7
#define ESP32_COMP_VIDEO_IMAGE_PTT        8
#define ESP32_COMP_VIDEO_IMAGE_SMPTE      9

class ESP32CompositeVideo_Class {
  public:
     ESP32CompositeVideo_Class(TwoWire &wire = Wire, uint32_t current_speed = 400000);
     void Init(uint8_t show_info = 0);
     uint8_t ChipPresent(void) { return _chip_present; }
     uint8_t ImageCount(void)  { return _nr_images;    }
     void ShowInfo(void);
     void ShowImage(uint8_t _image);
     void SetTextLineA(const char *s, uint8_t append = 0);
     void SetTextLineB(const char *s, uint8_t append = 0);
     void SendCommand(const char * cmd, const char * param = NULL);
  protected:
    void SetWireClock(void)     { if(_prevspeed != ESP32_COMP_VIDEO_I2C_CLOCK_SPEED) _wire->setClock(ESP32_COMP_VIDEO_I2C_CLOCK_SPEED); }
    void RestoreWireClock(void) { if(_prevspeed != ESP32_COMP_VIDEO_I2C_CLOCK_SPEED) _wire->setClock(_prevspeed); }

    TwoWire * _wire;
    uint32_t _prevspeed;
    uint8_t  _chip_present;
    uint8_t  _nr_images;
};

extern ESP32CompositeVideo_Class ESP32CompositeVideo;

#endif
