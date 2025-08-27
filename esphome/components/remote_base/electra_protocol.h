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
            uint8_t unused1; // byte 0
            uint8_t : 2;  // byte 1
            bool on : 1;
            uint8_t : 3;
            bool display_off : 1;
            bool eco : 1;
            uint8_t mode : 4; // byte 2
            bool health : 1;
            uint8_t : 3;
            uint8_t temperature : 4; // byte 3
            uint8_t : 4;
            uint8_t fan : 3;  // byte 4
            uint8_t swing_vertical : 3;
            uint8_t : 2;
            uint8_t unused3 [3]; // bytes 5,6,7
            uint8_t : 3; // byte 8
            bool swing_horizontal : 1;
            uint8_t : 4;
            uint8_t checksum; // byte 9
            uint8_t unused4 [2]; // bytes 10,11
      } fields;
  } payload;


  bool operator==(const ElectraData &rhs) const {
    return magic == rhs.magic && payload.words[0] == rhs.payload.words[0] &&
           payload.words[1] == rhs.payload.words[1] &&
           payload.words[2] == rhs.payload.words[2];
  }

  uint8_t temperature() const { return 31 - payload.fields.temperature; }
  void set_temperature(uint8_t temp) { payload.fields.temperature = 31 - temp; }
  bool checksum_ok() const {
    uint8_t sum = 0x15;
    for (int i = 0; i < 9; i++) {
      sum += payload.bytes[i];
    }
    return sum == payload.fields.checksum;
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
