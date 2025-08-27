#include "electra_ac.h"
#include "esphome/core/log.h"

namespace esphome {
namespace electra_ac {

static const char *const TAG = "electra_ac.climate";

bool ElectraClimate::on_receive(remote_base::RemoteReceiveData data) {
  remote_base::ElectraProtocol protocol;
  auto maybe_data = protocol.decode(data);
  if (!maybe_data.has_value()) {
    ESP_LOGW(TAG, "Received invalid Electra AC message");
    return false;
  }
  auto ir_message = maybe_data.value();
  if (!ir_message.checksum_ok()) {
    ESP_LOGW(TAG, "Received Electra AC message with invalid checksum");
    return false;
  }

  ESP_LOGD(TAG, "Received Electra AC message:");
  protocol.dump(ir_message);

  // Power
  if (!ir_message.payload.fields.on) {
    this->mode = climate::CLIMATE_MODE_OFF;
    this->publish_state();
    return true;
  }

  // Temperature
  this->target_temperature = ir_message.temperature();

  // Mode
  this->mode = ir_message.get_mode();

  // Fan
  this->fan_mode = ir_message.get_fan_mode();

  // Swing
  this->swing_mode = ir_message.get_swing_mode();

  if (ir_message.payload.fields.eco) {
    this->preset = climate::CLIMATE_PRESET_ECO;
  } else if (ir_message.payload.fields.turbo) {
    this->preset = climate::CLIMATE_PRESET_BOOST;
  } else if (ir_message.sleep_mode()) {
    this->preset = climate::CLIMATE_PRESET_SLEEP;
  } else {
    this->preset = climate::CLIMATE_PRESET_NONE;
  }

  this->publish_state();
  return true;
}

void ElectraClimate::transmit_state() {

}

} // namespace electra_ac
}  // namespace esphome