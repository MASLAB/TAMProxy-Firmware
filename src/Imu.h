#ifndef IMU_H
#define IMU_H

#include <cstdint>
#include <vector>
#include "Device.h"
#include "SparkFun_BNO08x_Arduino_Library.h"

namespace tamproxy {

class Imu: public Device {
private:
  BNO08x *sensor;
  bool init;
  uint8_t _dipin;
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  int16_t mx, my, mz;
public:
  Imu(uint8_t dipin);
  ~Imu();
  std::vector<uint8_t> handleRequest(std::vector<uint8_t> &request);
  void doUpkeep();
};

}

#endif
