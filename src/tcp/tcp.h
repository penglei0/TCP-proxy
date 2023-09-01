#ifndef SRC_TCP_TCP_H_
#define SRC_TCP_TCP_H_

#include <memory>
#include <unordered_map>

#include "base/channel.h"
#include "base/channel_tcp.h"
#include "base/rx_tx_interface.h"
#include "base/types.h"

/// @brief The manager of the `T` connections.
/// @tparam T
template <typename T>
class ConnectionManager {
 public:
  virtual ~ConnectionManager() {}
  virtual int AcceptConnection(int fd) = 0;
  virtual bool CloseConnection(const T& info) = 0;
  virtual auto GetConnection(uint64_t key) -> T = 0;
};

/// @brief rx/tx interface.
using RxNotifierPtr = std::shared_ptr<RxNotifier<TcpConnection>>;
using RxInterfacePtr = std::shared_ptr<RxInterface<TcpConnection>>;
using TxInterfacePtr = std::shared_ptr<TxInterface<TcpConnection>>;

/// @brief The manager of the TCP connections, with interface of rx/tx/accept.
class TcpConnMgr : public RxInterface<TcpConnection>,
                   public TxInterface<TcpConnection>,
                   public RxNotifier<TcpConnection>,
                   public ConnectionManager<TcpConnection> {
 public:
  TcpConnMgr();
  ~TcpConnMgr();
  /// @brief Accept a new connection from the `fd`.
  /// @param fd
  /// @return
  int AcceptConnection(int fd) override;
  /// @brief Close the connection with the `info`.
  /// @param info The information of the connection.
  /// @return
  bool CloseConnection(const TcpConnection& info) override;
  /// @brief Get the connection with the `key`.
  /// @param key
  /// @return
  auto GetConnection(uint64_t key) -> TcpConnection override;
  /// @brief Receive the packet from the `fd`.
  /// @param fd A file descriptor of the TCP connection.
  /// @return
  bool ReceiveMessage(int fd) override;
  /// @brief Send the packet to its associated connection.
  /// @param packet The packet to be sent.
  /// @param connectionInfo The connection information of the packet.
  /// @return
  bool SendMessage(const PacketPtr& packet,
                   const TcpConnection& connectionInfo) override;
  /// @brief Other modules will call this function to notify the reception of
  /// new packets.
  bool OnNewMessage(const PacketPtr& packet,
                    const TcpConnection& info) override;
  /// @brief Set the rx notifier.
  /// @param notifier
  void SetRxNotifier(const RxNotifierPtr& notifier) override;

 private:
  // the file descriptor of the TCP server.
  int server_fd_ = 0;
  // to notify the reception of new packets.
  RxNotifierPtr notifier_ = nullptr;
  std::unordered_map<uint64_t, TCPChannel> connections_;
};
using TcpConnMgrPtr = std::shared_ptr<TcpConnMgr>;

#endif  // SRC_TCP_TCP_H_
