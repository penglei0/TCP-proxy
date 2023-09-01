#ifndef SRC_BASE_RX_TX_INTERFACE_H_
#define SRC_BASE_RX_TX_INTERFACE_H_
#include <functional>

#include "base/packet.h"

using RegisterFunc = std::function<bool(int)>;
/// @brief The interface of other modules to notify the reception of new
/// packets.
/// @param T
template <typename T>
class RxNotifier {
 public:
  virtual ~RxNotifier() = default;
  virtual bool OnNewMessage(const PacketPtr& packet, const T& info) = 0;
};

/// @brief The interface to notify the transmission of sent
/// packet.
/// @param T
template <typename T>
class TxNotifier {
 public:
  virtual ~TxNotifier() = default;
  virtual bool HasTransmitted(const T& info) = 0;
};

/// @brief interface of handling the register/unregister
class PollerInterface {
 public:
  virtual ~PollerInterface() = default;
  virtual bool RegisterToPoller(RegisterFunc&& register_func) = 0;
  virtual bool UnregisterFromPoller(RegisterFunc&& unregister_func) = 0;
};

/// @brief The interface of rx/tx packets.
/// @param T The information of the operated packets.
template <typename T>
class RxInterface {
 public:
  virtual ~RxInterface() = default;
  virtual bool ReceiveMessage(int fd) = 0;
  virtual void SetRxNotifier(
      const std::shared_ptr<RxNotifier<T>>& notifier) = 0;
};

template <typename T>
class TxInterface {
 public:
  virtual ~TxInterface() = default;
  virtual bool SendMessage(const PacketPtr& packet, const T& info) = 0;
};

#endif  // SRC_BASE_RX_TX_INTERFACE_H_
