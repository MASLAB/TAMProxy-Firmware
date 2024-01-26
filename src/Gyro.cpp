#include "Gyro.h"
#include "config.h"

namespace tamproxy {

Gyro::Gyro(uint8_t sa0pin, int range) {
  init = false;
  _sa0pin = sa0pin;
  pinMode(_sa0pin, OUTPUT);
  digitalWrite(_sa0pin, LOW);
  gyro = new Adafruit_L3GD20();
  if (range == 1) {
    init = gyro->begin(gyro->L3DS20_RANGE_250DPS, 0x6A);
  }
  else if (range == 2) {
    init = gyro->begin(gyro->L3DS20_RANGE_500DPS, 0x6A);
  }
  else if (range == 3) {
    init = gyro->begin(gyro->L3DS20_RANGE_2000DPS, 0x6A);
  }
  digitalWrite(_sa0pin, HIGH);
}

Gyro::~Gyro() {
  delete gyro;
}

std::vector<uint8_t> Gyro::handleRequest(std::vector<uint8_t> &request) {
  if (request.size() < 1) {
    return {REQUEST_LENGTH_INVALID_CODE};    
  } else if (request[0] == GYRO_READ_CODE) {
    if(init) {
      digitalWrite(_sa0pin, LOW);
      gyro->read();
      digitalWrite(_sa0pin, HIGH);
      int16_t x = static_cast<int16_t>(gyro->data.x*1000);
      int16_t y = static_cast<int16_t>(gyro->data.y*1000);
      int16_t z = static_cast<int16_t>(gyro->data.z*1000);
      
      return {
        static_cast<uint8_t>((x >> 8) & 0xff),
        static_cast<uint8_t>(x & 0xff),
        static_cast<uint8_t>((y >> 8) & 0xff),
        static_cast<uint8_t>(y & 0xff),
        static_cast<uint8_t>((z >> 8) & 0xff),
        static_cast<uint8_t>(z & 0xff),
      };
    } else {
      return {0,0,0,0,0,0};
    }
    
  } else {
    return {REQUEST_BODY_INVALID_CODE};
  }
}

}
