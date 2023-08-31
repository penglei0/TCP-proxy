#ifndef SRC_TCP_TCP_H_
#define SRC_TCP_TCP_H_

#include <memory>
#include <unordered_map>

#include "base/channel.h"
#include "base/channel_tcp.h"
#include "base/rx_tx_interface.h"
#include "base/types.h"
#include "proxy_tunnel.h"

int CreateTcpServer(int port) { return 0; }

template <typename T>
class ConnectionManager {
 public:
  virtual ~ConnectionManager() {}
  virtual int AcceptConnection(int fd) = 0;
  virtual bool CloseConnection(const T& info) = 0;
  virtual auto GetConnection(uint64_t key) -> T = 0;
};

// TODO merge the implementation of TcpConnMgr and ProxyTunnel, they are the
// same things.
/// channel + serializer + deserializer = proxy tunnel = protocol
/// channel + ... + channel  = tcp connection manager
using RxNotifierPtr = std::shared_ptr<RxNotifier<TcpConnection>>;
using RxInterfacePtr = std::shared_ptr<RxInterface<TcpConnection>>;
using TxInterfacePtr = std::shared_ptr<TxInterface<TcpConnection>>;

class TcpConnMgr : public RxInterface<TcpConnection>,
                   public TxInterface<TcpConnection>,
                   public RxNotifier<TcpConnection>,
                   public ConnectionManager<TcpConnection> {
 public:
  TcpConnMgr() { server_fd_ = CreateTcpServer(10086); }
  ~TcpConnMgr() {}
  int AcceptConnection(int fd) override { return 0; }
  bool CloseConnection(const TcpConnection& info) override { return false; }
  auto GetConnection(uint64_t key) -> TcpConnection override {
    return TcpConnection();
  }
  bool ReceiveMessage(int fd) override {
    // TODO find the tcp channel by fd.
    TCPChannelPtr tcp = nullptr;
    auto packet = tcp->Read();
    if (packet) {
      const auto& conn = tcp->ConnectionInfo();
      notifier_->OnNewMessage(packet, conn);
    }
  }
  bool SendMessage(const PacketPtr& packet,
                   const TcpConnection& connectionInfo) override {
    return false;
  }
  /// @brief Other modules will call this function to notify the reception of
  /// new packets.
  bool OnNewMessage(const PacketPtr& packet,
                    const TcpConnection& info) override {
    return SendMessage(packet, info);
  }
  void SetRxNotifier(const RxNotifierPtr& notifier) override {
    notifier_ = notifier;
  }

 private:
  int server_fd_ = 0;
  std::unordered_map<uint64_t, TCPChannel> connections_;
  RxNotifierPtr notifier_ = nullptr;  // to notify the reception of new packets.
};
using TcpConnMgrPtr = std::shared_ptr<TcpConnMgr>;

#endif  // SRC_TCP_TCP_H_
