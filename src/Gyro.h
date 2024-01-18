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
public:
  Gyro();
  ~Gyro();
  std::vector<uint8_t> handleRequest(std::vector<uint8_t> &request);
};

}

#endif
