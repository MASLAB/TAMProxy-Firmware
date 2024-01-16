#ifndef IMU_H
#define IMU_H

#include <cstdint>
#include <vector>
#include "Device.h"
#include "MPU9250.h"
#include "MPU6050.h"

namespace tamproxy {

class Imu: public Device {
private:
  MPU9250 *sensor;
public:
  Imu();
  ~Imu();
  std::vector<uint8_t> handleRequest(std::vector<uint8_t> &request);
};

}

#endif
