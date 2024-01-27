#ifndef COLOR_H
#define COLOR_H

#include <cstdint>
#include <vector>
#include "Device.h"
#include "Adafruit_AS726x.h"

namespace tamproxy {

class Color : public Device {
private:
  Adafruit_AS726x *ams;
  uint16_t sensorValues[AS726x_NUM_CHANNELS];
  bool init;
  bool ready;
  uint8_t count;
  uint16_t v, b, g, y, o, r;
public:
  Color();
  ~Color();
  std::vector<uint8_t> handleRequest(std::vector<uint8_t> &request);
  void doUpkeep();
};
  
}

#endif
