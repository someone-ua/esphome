#pragma once

#include "remote_base.h"

namespace esphome {
namespace remote_base {

struct ElectraData {
  uint64_t value1;
  uint64_t value2;

  bool operator==(const ElectraData &rhs) const { return value1 == rhs.value1 && value2 == rhs.value2; }
};

class ElectraProtocol : public RemoteProtocol<ElectraData> {
 public:
  void encode(RemoteTransmitData *dst, const ElectraData &data) override;
  optional<ElectraData> decode(RemoteReceiveData src) override;
  void dump(const ElectraData &data) override;
};

DECLARE_REMOTE_PROTOCOL(Electra)

template<typename... Ts> class ElectraAction : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(uint16_t, address)
  TEMPLATABLE_VALUE(uint16_t, command)
  TEMPLATABLE_VALUE(uint16_t, command_repeats)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    ElectraData data{};
    data.address = this->address_.value(x...);
    data.command = this->command_.value(x...);
    data.command_repeats = this->command_repeats_.value(x...);
    ElectraProtocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
