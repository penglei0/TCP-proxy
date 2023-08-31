#ifndef SRC_BASE_PROTO_H_
#define SRC_BASE_PROTO_H_

#include "packet.h"
#include "types.h"

/// @brief The interface of proto.
/// @tparam T The information of the proto header.
template <typename T>
class ProtoInterface {
 public:
  virtual ~ProtoInterface() = default;
  /// @brief Parse the packet to the proto header. And return the information of
  /// the proto header.
  /// @param packet
  /// @return
  virtual T Parse(const PacketPtr& packet) = 0;
  virtual PacketPtr Build(const PacketPtr& packet) = 0;
};

#endif  // SRC_BASE_PROTO_H_