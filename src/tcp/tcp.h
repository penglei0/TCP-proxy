#ifndef SRC_TCP_TCP_H_
#define SRC_TCP_TCP_H_

#include <memory>
#include <unordered_map>

#include "base/channel.h"
#include "base/types.h"

/// @brief the message format between the proxy client and proxy server.
class TcpConnection {
 public:
  uint32_t src_ip;
  uint16_t src_port;
  uint32_t dst_ip;
  uint16_t dst_port;
  int fd;
  int type;
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

/* how to manage connections and downlink channls(TCP channels.)
class TcpConnectionManager {
 public:
  TcpConnectionManager() = default;
  ~TcpConnectionManager() {}

  auto AcceptConnection(int fd) -> TcpConnectionPtr { return 0; }
  bool CloseConnection(const TcpConnection& info) { return false; }
  auto GetConnection(uint64_t key) -> TcpConnectionPtr { return nullptr; }
  auto GetAllConnections()
      -> const std::unordered_map<uint64_t, TcpConnectionPtr>& {
    return connections_;
  }
  virtual PacketPtr Read() = 0;
  virtual bool Write(const PacketPtr& packet) = 0;
  virtual int GetFd() const = 0;

 private:
  std::unordered_map<uint64_t, TcpConnectionPtr> connections_;
};
using TcpConnectionManagerPtr = std::shared_ptr<TcpConnectionManager>;
*/

int CreateTcpServer(int port) { return 0; }

#endif  // SRC_TCP_TCP_H_
