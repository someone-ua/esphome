#pragma once

#include "remote_base.h"
#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome {
namespace remote_base {

#define ELECTRA_MAGIC 0x0126CB23

#define AC_MODE_FEEL 0x8
#define AC_MODE_COOL 0x3
#define AC_MODE_DRY 0x2
#define AC_MODE_FAN 0x7
#define AC_MODE_HEAT 0x1
#define AC_FAN_AUTO 0x0
#define AC_FAN_LOW 0x2
#define AC_FAN_MEDIUM 0x3
#define AC_FAN_HIGH 0x5
#define AC_FAN_SLEEP 0x1
#define AC_SWING_VERTICAL_ON 0x7
#define AC_SWING_VERTICAL_OFF 0x0

struct ElectraData {
  uint32_t magic;
  union
  {
      uint8_t bytes[12];
      uint32_t words[3];
      struct {
            // byte 0
            uint8_t unused1;
            // byte 1
            uint8_t : 2;
            bool on : 1;
            bool timer : 1;
            uint8_t : 2;
            bool display_off : 1;
            bool eco : 1;
            // byte 2
            uint8_t mode : 4;
            bool health : 1;
            uint8_t : 1;
            bool turbo : 1;
            uint8_t : 1;
            // byte 3
            uint8_t temperature : 4;
            uint8_t : 4;
            // byte 4
            uint8_t fan : 3;
            uint8_t swing_vertical : 3;
            uint8_t : 2;
            // byte 5
            uint8_t timer_deciminutes;
            // bytes 6,7
            uint8_t unused3 [2];
            // byte 8
            uint8_t : 3;
            bool swing_horizontal : 1;
            uint8_t : 4;
            // byte 9
            uint8_t checksum;
            // bytes 10,11
            uint8_t unused4 [2];
      } fields;
  } payload;


  bool operator==(const ElectraData &rhs) const {
    return magic == rhs.magic && payload.words[0] == rhs.payload.words[0] &&
           payload.words[1] == rhs.payload.words[1] &&
           payload.words[2] == rhs.payload.words[2];
  }

  uint8_t temperature() const { return 31 - payload.fields.temperature; }
  void set_temperature(uint8_t temp) { payload.fields.temperature = 31 - temp; }
  bool checksum_ok() const {
    uint8_t sum = 0x15;
    for (int i = 0; i < 9; i++) {
      sum += payload.bytes[i];
    }
    return sum == payload.fields.checksum;
  }
  esphome::climate::ClimateMode get_mode() const {
    switch (payload.fields.mode) {
      case AC_MODE_FEEL:
        return esphome::climate::CLIMATE_MODE_AUTO;
      case AC_MODE_COOL:
        return esphome::climate::CLIMATE_MODE_COOL;
      case AC_MODE_DRY:
        return esphome::climate::CLIMATE_MODE_DRY;
      case AC_MODE_FAN:
        return esphome::climate::CLIMATE_MODE_FAN_ONLY;
      case AC_MODE_HEAT:
        return esphome::climate::CLIMATE_MODE_HEAT;
      default:
        return esphome::climate::CLIMATE_MODE_OFF;
    }
  }

  void set_mode(esphome::climate::ClimateMode mode) {
    switch (mode) {
      case esphome::climate::CLIMATE_MODE_AUTO:
        payload.fields.mode = AC_MODE_FEEL;
        break;
      case esphome::climate::CLIMATE_MODE_COOL:
        payload.fields.mode = AC_MODE_COOL;
        break;
      case esphome::climate::CLIMATE_MODE_DRY:
        payload.fields.mode = AC_MODE_DRY;
        break;
      case esphome::climate::CLIMATE_MODE_FAN_ONLY:
        payload.fields.mode = AC_MODE_FAN;
        break;
      case esphome::climate::CLIMATE_MODE_HEAT:
        payload.fields.mode = AC_MODE_HEAT;
        break;
      default:
        payload.fields.mode = AC_MODE_COOL;
        break;
    }
  }

  esphome::climate::ClimateFanMode get_fan_mode() const {
    switch (payload.fields.fan) {
      case AC_FAN_AUTO:
        return esphome::climate::CLIMATE_FAN_AUTO;
      case AC_FAN_LOW:
        return esphome::climate::CLIMATE_FAN_LOW;
      case AC_FAN_MEDIUM:
        return esphome::climate::CLIMATE_FAN_MEDIUM;
      case AC_FAN_HIGH:
        return esphome::climate::CLIMATE_FAN_HIGH;
      case AC_FAN_SLEEP:
        return esphome::climate::CLIMATE_FAN_QUIET;
      default:
        return esphome::climate::CLIMATE_FAN_AUTO;
    }
  }

  void set_fan_mode(esphome::climate::ClimateFanMode fan_mode) {
    switch (fan_mode) {
      case esphome::climate::CLIMATE_FAN_AUTO:
        payload.fields.fan = AC_FAN_AUTO;
        break;
      case esphome::climate::CLIMATE_FAN_LOW:
        payload.fields.fan = AC_FAN_LOW;
        break;
      case esphome::climate::CLIMATE_FAN_MEDIUM:
        payload.fields.fan = AC_FAN_MEDIUM;
        break;
      case esphome::climate::CLIMATE_FAN_HIGH:
        payload.fields.fan = AC_FAN_HIGH;
        break;
      case esphome::climate::CLIMATE_FAN_QUIET:
      case esphome::climate::CLIMATE_FAN_MIDDLE:
      case esphome::climate::CLIMATE_FAN_FOCUS:
      case esphome::climate::CLIMATE_FAN_DIFFUSE:
        payload.fields.fan = AC_FAN_SLEEP; // map unsupported modes to sleep
        break;
      default:
        payload.fields.fan = AC_FAN_AUTO;
        break;
    }
  }

  esphome::climate::ClimateSwingMode get_swing_mode() const {
    if (payload.fields.swing_vertical == AC_SWING_VERTICAL_OFF && !payload.fields.swing_horizontal) {
      return esphome::climate::CLIMATE_SWING_OFF;
    } else if (payload.fields.swing_vertical == AC_SWING_VERTICAL_OFF && payload.fields.swing_horizontal) {
      return esphome::climate::CLIMATE_SWING_HORIZONTAL;
    } else if (payload.fields.swing_vertical == AC_SWING_VERTICAL_ON && !payload.fields.swing_horizontal) {
      return esphome::climate::CLIMATE_SWING_VERTICAL;
    } else {
      return esphome::climate::CLIMATE_SWING_BOTH;
    }
  }

  void set_swing_mode(esphome::climate::ClimateSwingMode swing_mode) {
    switch (swing_mode) {
      case esphome::climate::CLIMATE_SWING_OFF:
        payload.fields.swing_vertical = AC_SWING_VERTICAL_OFF;
        payload.fields.swing_horizontal = false;
        break;
      case esphome::climate::CLIMATE_SWING_HORIZONTAL:
        payload.fields.swing_vertical = AC_SWING_VERTICAL_OFF;
        payload.fields.swing_horizontal = true;
        break;
      case esphome::climate::CLIMATE_SWING_VERTICAL:
        payload.fields.swing_vertical = AC_SWING_VERTICAL_ON;
        payload.fields.swing_horizontal = false;
        break;
      case esphome::climate::CLIMATE_SWING_BOTH:
        payload.fields.swing_vertical = AC_SWING_VERTICAL_ON;
        payload.fields.swing_horizontal = true;
        break;
      default:
        payload.fields.swing_vertical = AC_SWING_VERTICAL_OFF;
        payload.fields.swing_horizontal = false;
        break;
    }
  }

  bool sleep_mode() const {
    return payload.fields.fan == AC_FAN_SLEEP;
  }

  void set_checksum() {
    uint8_t sum = 0x15;
    for (int i = 0; i < 9; i++) {
      sum += payload.bytes[i];
    }
    payload.fields.checksum = sum;
  }
};

class ElectraProtocol : public RemoteProtocol<ElectraData> {
 public:
  void encode(RemoteTransmitData *dst, const ElectraData &data) override;
  optional<ElectraData> decode(RemoteReceiveData src) override;
  void dump(const ElectraData &data) override;
};

DECLARE_REMOTE_PROTOCOL(Electra)

}  // namespace remote_base
}  // namespace esphome
