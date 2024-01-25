#ifndef GYRO_H
#define GYRO_H

#include <cstdint>
#include <vector>
#include "Device.h"
#include "Adafruit_L3GD20.h"

namespace tamproxy {

class Gyro : public Device {
private:
  Adafruit_L3GD20 *gyro;
  bool init;
  uint8_t _sa0pin;
public:
  Gyro(uint8_t sa0pin, int range);
  ~Gyro();
  std::vector<uint8_t> handleRequest(std::vector<uint8_t> &request);
};

}

#endif
