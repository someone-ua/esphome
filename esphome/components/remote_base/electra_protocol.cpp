#include "electra_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.electra";

static const uint32_t HEADER_HIGH_US = 3120;
static const uint32_t HEADER_LOW_US = 1570;
static const uint32_t BIT_HIGH_US = 560;
static const uint32_t BIT_ONE_LOW_US = 1060;
static const uint32_t BIT_ZERO_LOW_US = 260;

void ElectraProtocol::encode(RemoteTransmitData *dst, const ElectraData &data) {

}

optional<ElectraData> ElectraProtocol::decode(RemoteReceiveData src) {
  ElectraData data{
      .magic = 0,
      .payload = {.words = {0, 0, 0}},
  };
  if (!src.expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return {};

  for (uint32_t mask = 1; mask; mask <<= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.magic |= mask;
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.magic &= ~mask;
    } else {
      return {};
    }
  }

  for (int i = 0; i < 12; i++) {
    for (uint8_t mask = 1; mask; mask <<= 1) {
      if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
        data.payload.bytes[i] |= mask;
      } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
        data.payload.bytes[i] &= ~mask;
      } else {
        break;
      }
    }
  }

  return data;
}
void ElectraProtocol::dump(const ElectraData &data) {
  ESP_LOGI(TAG, "Received Electra: magic=0x%08X", data.magic);

  ESP_LOGI(TAG, " bytes: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
           data.payload.bytes[0], data.payload.bytes[1],
           data.payload.bytes[2], data.payload.bytes[3], data.payload.bytes[4], data.payload.bytes[5],
           data.payload.bytes[6], data.payload.bytes[7], data.payload.bytes[8], data.payload.bytes[9],
           data.payload.bytes[10], data.payload.bytes[11]);
  ESP_LOGI(TAG, " on: %s",
           data.payload.fields.on ? "yes" : "no");
  ESP_LOGI(TAG, " temperature: %d",
           data.temperature());
  ESP_LOGI(TAG, " display: %s",
           data.payload.fields.display_off ? "no" : "yes");
  ESP_LOGI(TAG, " eco: %s",
           data.payload.fields.eco ? "yes" : "no");
  ESP_LOGI(TAG, " mode: 0x%X",
           data.payload.fields.mode);
  ESP_LOGI(TAG, " health: %s",
           data.payload.fields.health ? "yes" : "no");
  ESP_LOGI(TAG, " fan: 0x%X",
           data.payload.fields.fan);
}

}  // namespace remote_base
}  // namespace esphome
