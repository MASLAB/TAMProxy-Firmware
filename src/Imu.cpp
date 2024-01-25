#include "Imu.h"
#include <cstdint>
#include "Wire.h"
#include "config.h"

namespace tamproxy {

Imu::Imu(uint8_t dipin) {
  init = false;
  _dipin = dipin;
  sensor = new BNO08x();
  pinMode(_dipin, OUTPUT);
  digitalWrite(_dipin, HIGH);
  delay(10);
  Wire.begin();
  init = sensor->begin(0x4B, Wire, -1, -1);
  init &= sensor->enableLinearAccelerometer();
  init &= sensor->enableGyro(SH2_GYROSCOPE_CALIBRATED);
  init &= sensor->enableMagnetometer(SH2_MAGNETIC_FIELD_CALIBRATED);
  digitalWrite(_dipin, LOW);
}

Imu::~Imu() {
  // delete sensor;
}

std::vector<uint8_t> Imu::handleRequest(std::vector<uint8_t> &request) {
  if (request.size() != 1) {
    return {REQUEST_LENGTH_INVALID_CODE};
  } else if (request[0] != IMU_READ_CODE) {
    return {REQUEST_BODY_INVALID_CODE};
  } else {
    if(init) {
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
    } else {
      return {
        0,0,0,0,0,0,
        0,0,0,0,0,0,
        0,0,0,0,0,0
      };
    }
  }
}

void Imu::doUpkeep() {
  digitalWrite(_dipin, HIGH);
  if(init) {
    if(sensor->getSensorEvent()) {
      // Multiplying all these float results to 1000 for extra range
      switch (sensor->getSensorEventID()) {
        case SENSOR_REPORTID_LINEAR_ACCELERATION: // m/s^2 * 1000
          ax = static_cast<int16_t>(sensor->getLinAccelX()*1000);
          ay = static_cast<int16_t>(sensor->getLinAccelY()*1000);
          az = static_cast<int16_t>(sensor->getLinAccelZ()*1000);
          break;
        case SENSOR_REPORTID_GYROSCOPE_CALIBRATED: // rad/s * 1000
          gx = static_cast<int16_t>(sensor->getGyroX()*1000);
          gy = static_cast<int16_t>(sensor->getGyroY()*1000);
          gz = static_cast<int16_t>(sensor->getGyroZ()*1000);
          break;
        case SENSOR_REPORTID_MAGNETIC_FIELD: // uTesla * 1000
          mx = static_cast<int16_t>(sensor->getMagX()*1000);
          my = static_cast<int16_t>(sensor->getMagY()*1000);
          mz = static_cast<int16_t>(sensor->getMagZ()*1000);   
          break;
      }
    }
  }
  digitalWrite(_dipin, LOW);
}

}
