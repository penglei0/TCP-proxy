#ifndef SRC_BASE_RX_TX_ENTITY_H_
#define SRC_BASE_RX_TX_ENTITY_H_

#include "base/rx_tx_interface.h"
#include "types.h"

template <typename T>
class RxTxEntity : public RxInterface<T>,
                   public TxInterface<T>,
                   public RxNotifier<T>,
                   public PollerInterface {
 public:
  virtual ~RxTxEntity() = default;
  bool RegisterToPoller(RegisterFunc&& register_func) {
    register_func_ = std::move(register_func);
    return true;
  }
  bool UnregisterFromPoller(RegisterFunc&& unregister_func) {
    unregister_func_ = std::move(unregister_func);
    return true;
  }

 protected:
  RegisterFunc register_func_;
  RegisterFunc unregister_func_;
};

/// @brief rx/tx interface.
using RxNotifierPtr = std::shared_ptr<RxNotifier<TcpConnection>>;
using RxTxEntityTcpPtr = std::shared_ptr<RxTxEntity<TcpConnection>>;

#endif  // SRC_BASE_RX_TX_ENTITY_H_