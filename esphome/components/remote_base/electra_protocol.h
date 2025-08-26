#pragma once

#include "remote_base.h"

namespace esphome {
namespace remote_base {

struct ElectraData {
  uint64_t magic;
  union payload
  {
      uint64_t value;
      struct {
          uint something : 40;
          uint temperature : 8;
      };
  } payload;


  bool operator==(const ElectraData &rhs) const { return magic == rhs.magic && payload.value == rhs.payload.value; }
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
  TEMPLATABLE_VALUE(uint64_t, value1)
  TEMPLATABLE_VALUE(uint64_t, value2)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    ElectraData data{};
    data.value1 = this->value1_.value(x...);
    data.value2 = this->value2_.value(x...);
    ElectraProtocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
