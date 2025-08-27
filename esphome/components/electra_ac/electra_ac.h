#pragma once

#include "esphome/components/climate_ir/climate_ir.h"
#include "esphome/components/remote_base/electra_protocol.h"

namespace esphome {
namespace electra_ac {

#define AC1_TEMP_MIN 16
#define AC1_TEMP_MAX 31
#define AC1_TEMP_INC 1

class ElectraClimate : public climate_ir::ClimateIR {
 public:
  ElectraClimate()
      : climate_ir::ClimateIR(
            AC1_TEMP_MIN, AC1_TEMP_MAX, AC1_TEMP_INC, true, true,
            {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
             climate::CLIMATE_FAN_HIGH, climate::CLIMATE_FAN_SLEEP},
            {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_SWING_HORIZONTAL,
             climate::CLIMATE_SWING_BOTH},
            {climate::CLIMATE_PRESET_NONE, climate::CLIMATE_PRESET_ECO, climate::CLIMATE_PRESET_BOOST}) {}

  void setup() override { climate_ir::ClimateIR::setup(); }

 protected:
  /// Transmit via IR the state of this climate controller.
  void transmit_state() override;
  /// Handle received IR Buffer
  bool on_receive(remote_base::RemoteReceiveData data) override;
 private:
  remote_base::ElectraProtocol protocol{};
};

}  // namespace electra_ac
}  // namespace esphome