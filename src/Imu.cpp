#include "Imu.h"
#include <cstdint>
#include "MPU9250.h"
#include "config.h"

#define I2Cclock 400000
#define I2Cport Wire
#define MPU9250_ADDRESS MPU9250_ADDRESS_AD0

namespace tamproxy {

Imu::Imu() {
  sensor = new MPU9250(MPU9250_ADDRESS, I2Cport, I2Cclock);

  sensor->calibrateMPU9250(sensor->gyroBias, sensor->accelBias);
  sensor->initMPU9250();
  sensor->initAK8963(sensor->factoryMagCalibration);
}

Imu::~Imu() {
  delete sensor;
}

std::vector<uint8_t> Imu::handleRequest(std::vector<uint8_t> &request) {
  if (request.size() != 1) {
    return {REQUEST_LENGTH_INVALID_CODE};
  } else if (request[0] != IMU_READ_CODE) {
    return {REQUEST_BODY_INVALID_CODE};
  } else {
    uint16_t ax, ay, az;
    uint16_t gx, gy, gz;
    uint16_t mx, my, mz;

    sensor->readAccelData(sensor->accelCount);  // Read the x/y/z adc values

    ax = sensor->accelCount[0];
    ay = sensor->accelCount[1];
    az = sensor->accelCount[2];

    sensor->readGyroData(sensor->gyroCount);  // Read the x/y/z adc values

    gx = sensor->gyroCount[0];
    gy = sensor->gyroCount[1];
    gz = sensor->gyroCount[2];

    sensor->readMagData(sensor->magCount);  // Read the x/y/z adc values

    mx = sensor->magCount[0] * sensor->factoryMagCalibration[0];
    my = sensor->magCount[1] * sensor->factoryMagCalibration[1];
    mz = sensor->magCount[2] * sensor->factoryMagCalibration[2];

    return {
      static_cast<uint8_t>((ax >> 8) & 0xff),
      static_cast<uint8_t>(ax & 0xff),
      static_cast<uint8_t>((ay >> 8) & 0xff),
      static_cast<uint8_t>(ay & 0xff),
      static_cast<uint8_t>((az >> 8) & 0xff),
      static_cast<uint8_t>(az & 0xff),
      static_cast<uint8_t>((gx >> 8) & 0xff),
      static_cast<uint8_t>(gx & 0xff),
      static_cast<uint8_t>((gy >> 8) & 0xff),
      static_cast<uint8_t>(gy & 0xff),
      static_cast<uint8_t>((gz >> 8) & 0xff),
      static_cast<uint8_t>(gz & 0xff),
      static_cast<uint8_t>((mx >> 8) & 0xff),
      static_cast<uint8_t>(mx & 0xff),
      static_cast<uint8_t>((my >> 8) & 0xff),
      static_cast<uint8_t>(my & 0xff),
      static_cast<uint8_t>((mz >> 8) & 0xff),
      static_cast<uint8_t>(mz & 0xff)
    };
  }
}

}
