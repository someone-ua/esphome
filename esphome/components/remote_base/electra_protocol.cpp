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
      .value1 = 0,
      .value2 = 0,
  };
  ESP_LOGD(TAG, "Decoding Electra with %d entries", src.size());
  if (!src.expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return {};

  ESP_LOGD(TAG, "Header matched");
  for (uint64_t mask = 1; mask; mask <<= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.value1 |= mask;
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.value1 &= ~mask;
    } else {
      ESP_LOGD(TAG, "Value1 decoding failed at mask 0x%04X", mask);
      return {};
    }
  }

  ESP_LOGD(TAG, "Value1 decoded: 0x%04X", data.value1);

  ESP_LOGD(TAG, "Header matched");
  for (uint64_t mask = 1; mask; mask <<= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.value2 |= mask;
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.value2 &= ~mask;
    } else {
      ESP_LOGD(TAG, "Value2 decoding failed at mask 0x%04X", mask);
      return {};
    }
  }

  ESP_LOGD(TAG, "Value2 decoded: 0x%04X", data.value2);
  }

  return data;
}
void ElectraProtocol::dump(const ElectraData &data) {
  ESP_LOGI(TAG, "Received Electra: value1=0x%04X, value2=0x%04X", data.value1, data.value2);
}

}  // namespace remote_base
}  // namespace esphome
