#include "electra_ac.h"
#include "esphome/core/log.h"

namespace esphome {
namespace electra_ac {

static const char *const TAG = "electra_ac.climate";

bool ElectraClimate::on_receive(remote_base::RemoteReceiveData data) {
  auto maybe_data = this->protocol.decode(data);
  if (!maybe_data.has_value()) {
    return false;
  }
  auto ir_message = maybe_data.value();
  if (!ir_message.checksum_ok()) {
    ESP_LOGW(TAG, "Received Electra AC message with invalid checksum");
    return false;
  }

  ESP_LOGD(TAG, "Received Electra AC message:");
  this->protocol.dump(ir_message);

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
  } else {
    this->preset = climate::CLIMATE_PRESET_NONE;
  }

  this->publish_state();
  return true;
}

void ElectraClimate::transmit_state() {
    remote_base::ElectraData data{ .magic = ELECTRA_MAGIC, payload.fields.always_one = 1, payload.fields.always_one2 = 1 };
    // Set fields
    data.payload.fields.on = this->mode != climate::CLIMATE_MODE_OFF;
    data.payload.fields.display_off = false;  // TODO: Add support for display off
    data.payload.fields.health = false;       // TODO: Add support for health mode
    data.payload.fields.timer = false;        // TODO: Add support for timer
    data.payload.fields.timer_deciminutes = 0;

    if (this->target_temperature < AC1_TEMP_MIN)
        this->target_temperature = AC1_TEMP_MIN;
    if (this->target_temperature > AC1_TEMP_MAX)
        this->target_temperature = AC1_TEMP_MAX;
    data.set_temperature(this->target_temperature);

    data.set_mode(this->mode);
    data.set_fan_mode(this->fan_mode.value_or(climate::CLIMATE_FAN_AUTO));
    data.set_swing_mode(this->swing_mode);

    data.set_checksum();

    // Send the code
    auto transmit = this->transmitter_->transmit();
    auto *transmit_data = transmit.get_data();

    this->protocol.encode(transmit_data, data);
    transmit.perform();
}

} // namespace electra_ac
}  // namespace esphome