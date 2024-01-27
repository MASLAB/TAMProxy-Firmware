#include "Color.h"
#include <cstdint>
#include "config.h"

namespace tamproxy {

Color::Color() {
  init = false;
  ams = new Adafruit_AS726x();
  init = ams->begin();
}

Color::~Color() {
  delete ams;
}

std::vector<uint8_t> Color::handleRequest(std::vector<uint8_t> &request) {
  if (request.size() != 1) {
    return {REQUEST_LENGTH_INVALID_CODE};
  } else if (request[0] != COLOR_READ_CODE) {
    return {REQUEST_BODY_INVALID_CODE};
  } else {
    // Return v, b, g, y, o, r
    // Total: 12 bytes (6 uint16_t)
    if (init) {
      return {
        static_cast<uint8_t>((v >> 8) & 0xff),
        static_cast<uint8_t>(v & 0xff),
        static_cast<uint8_t>((b >> 8) & 0xff),
        static_cast<uint8_t>(b & 0xff),
        static_cast<uint8_t>((g >> 8) & 0xff),
        static_cast<uint8_t>(g & 0xff),
        static_cast<uint8_t>((y >> 8) & 0xff),
        static_cast<uint8_t>(y & 0xff),
        static_cast<uint8_t>((o >> 8) & 0xff),
        static_cast<uint8_t>(o & 0xff),
        static_cast<uint8_t>((r >> 8) & 0xff),
        static_cast<uint8_t>(r & 0xff)};
    }
    else {
      return {0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff};
    }
  }
}

void Color::doUpkeep() {
  if(init) {
    if(ready){
      ams->startMeasurement();
      ready = false;
      count = 0;
    }
    
    if(count == 0){ 
      ready = ams->dataReady();
      if(ready) {
        ams->readRawValues(sensorValues);
        v = sensorValues[AS726x_VIOLET];
        b = sensorValues[AS726x_BLUE];
        g = sensorValues[AS726x_GREEN];
        y = sensorValues[AS726x_YELLOW];
        o = sensorValues[AS726x_ORANGE];
        r = sensorValues[AS726x_RED];
      }
    } else {
      count++;
    }
  }
}
}
