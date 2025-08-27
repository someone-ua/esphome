#pragma once

#include "remote_base.h"

namespace esphome {
namespace remote_base {

struct ElectraData {
  uint32_t magic;
  union
  {
      uint8_t bytes[12];
      uint32_t words[3];
      struct {
            uint8_t [4];
            uint8_t temperature : 4;
            uint8_t : 4;
            uint8_t [7];
      } fields;
  } payload;


  bool operator==(const ElectraData &rhs) const {
    return magic == rhs.magic && payload.words[0] == rhs.payload.words[0] &&
           payload.words[1] == rhs.payload.words[1] &&
           payload.words[2] == rhs.payload.words[2];
  }
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
