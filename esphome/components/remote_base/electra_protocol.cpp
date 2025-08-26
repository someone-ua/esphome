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
      .payload = {.valint64 = 0},
  };
  if (!src.expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return {};

  for (uint64_t mask = 1; mask; mask <<= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.magic |= mask;
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.magic &= ~mask;
    } else {
      return {};
    }
  }

  int i = 0;
  for (uint64_t mask = 1; mask; mask <<= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.payload.valint64 |= mask;
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.payload.valint64 &= ~mask;
    } else {
      break;
    }
    i++;
  }

  return data;
}
void ElectraProtocol::dump(const ElectraData &data) {
  ESP_LOGI(TAG, " payload value: 0x%016llX", data.payload.valint64);
  ESP_LOGI(TAG, "Received Electra: magic=0x%016X, payload=0x%016X", data.magic, data.payload.valint64);
  ESP_LOGI(TAG, "Received Electra: payload=0x%016X  magic=0x%016X", data.payload.valint64, data.magic);
  ESP_LOGI(TAG, "Received Electra: magic=0x%016X, payload=0x%016X", data.magic, data.payload.valint64);

  ESP_LOGI(TAG, "magic=0x%016X,  bytes: %02X %02X %02X %02X %02X %02X %02X %02X", data.magic,
           data.payload.bytes[0], data.payload.bytes[1],
           data.payload.bytes[2], data.payload.bytes[3], data.payload.bytes[4], data.payload.bytes[5],
           data.payload.bytes[6], data.payload.bytes[7]);
  ESP_LOGI(TAG, "  words: %08X %08X", data.payload.words[0], data.payload.words[1]);
}

}  // namespace remote_base
}  // namespace esphome
