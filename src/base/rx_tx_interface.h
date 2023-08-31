#ifndef SRC_RX_TX_INTERFACE_H_
#define SRC_RX_TX_INTERFACE_H_

#include "base/packet.h"
/// @brief The interface of other modules to notify the reception of new
/// packets.
/// @tparam T
template <typename T>
class RxNotifier {
 public:
  virtual ~RxNotifier() = default;
  virtual bool OnNewMessage(const PacketPtr& packet, const T& info) = 0;
};

/// @brief The interface of rx/tx packets.
/// @tparam T The information of the operated packets.
template <typename T>
class RxTxInterface {
 public:
  virtual ~RxTxInterface() = default;
  virtual bool ReceiveMessage(int fd) = 0;
  virtual bool SendMessage(const PacketPtr& packet, const T& info) = 0;
  virtual void SetNotifier(const std::shared_ptr<RxNotifier<T>>& notifier) = 0;
};

#endif  // SRC_RX_TX_INTERFACE_H_