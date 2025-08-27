#include "electra_ac.h"
#include "electra_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace electra_ac {

static const char *const TAG = "electra_ac.climate";

bool ElectraClimate::on_receive(remote_base::RemoteReceiveData data) {
  auto maybe_data = remote_base::ElectraProtocol::decode(data);
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
  remote_base::ElectraProtocol::dump(ir_message);

  // Power
  if (!ir_message.payload.fields.on) {
    this->mode = climate::CLIMATE_MODE_OFF;
    this->publish_state();
    return true;
  }

  // Temperature
  this->target_temperature = ir_message.temperature();

  // Mode
  switch (ir_message.payload.fields.mode) {
    case 0:
      this->mode = climate::CLIMATE_MODE_AUTO;
      break;
    case 1:
      this->mode = climate::CLIMATE_MODE_COOL;
      break;
    case 2:
      this->mode = climate::CLIMATE_MODE_DRY;
      break;
    case 3:
      this->mode = climate::CLIMATE_MODE_FAN_ONLY;
      break;
    case 4:
      this->mode = climate::CLIMATE_MODE_HEAT;
      break;
    default:
      ESP_LOGW(TAG, "Received unknown Electra AC mode: %d", ir_message.payload.fields.mode);
      this->mode = climate::CLIMATE_MODE_OFF;
      break;
  }

  // Fan
  switch (ir_message.payload.fields.fan) {
    case 0:
      this->fan_mode = climate::CLIMATE_FAN_AUTO;
      break;
    case 1:
      this->fan_mode = climate::CLIMATE_FAN_LOW;
      break;
    case 2:
      this->fan_mode = climate::CLIMATE_FAN_MEDIUM;
      break;
    case 3:
      this->fan_mode = climate::CLIMATE_FAN_HIGH;
      break;
    default:
      ESP_LOGW(TAG, "Received unknown Electra AC fan speed: %d", ir_message.payload.fields.fan);
      this->fan_mode = climate::CLIMATE_FAN_AUTO;
      break;
  }

  // Swing
  if (ir_message.payload.fields.swing_vertical == 0 && ir_message.payload.fields.swing_horizontal == 0) {
    this->swing_mode = climate::CLIMATE_SWING_OFF;
  } else if (ir_message.payload.fields.swing_vertical == 0 && ir_message.payload.fields.swing_horizontal == 1) {
    this->swing_mode = climate::CLIMATE_SWING_HORIZONTAL;
  } else if (ir_message.payload.fields.swing_vertical == 1 && ir_message.payload.fields.swing_horizontal == 0) {
    this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
  } else {
    this->swing_mode = climate::CLIMATE_SWING_BOTH;
  }
}

void ElectraClimate::transmit_state() {

}

} // namespace electra_ac
}  // namespace esphome