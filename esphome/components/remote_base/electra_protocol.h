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
          uint temperature : 7;
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

// template<typename... Ts> class ElectraAction : public RemoteTransmitterActionBase<Ts...> {
//  public:
//   TEMPLATABLE_VALUE(uint64_t, magic)
//   TEMPLATABLE_VALUE(uint64_t, payload)

//   void encode(RemoteTransmitData *dst, Ts... x) override {
//     ElectraData data{};
//     data.magic = this->magic_.value(x...);
//     data.payload = this->data.payload_.value(x...);
//     ElectraProtocol().encode(dst, data);
//   }
// };

}  // namespace remote_base
}  // namespace esphome
