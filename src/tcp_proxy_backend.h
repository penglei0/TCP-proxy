/**
 * @file tcp_proxy_backend.h
 * @author Peng lei (peng.lei@n-hop.com)
 * @brief The backend of TCP proxy module is used to transmit the TCP payload
 * data to the remote by using UDP/KCP or other protocols.
 * @version 0.1
 * @date 2023-08-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef SRC_TCP_PROXY_BACKEND_H_
#define SRC_TCP_PROXY_BACKEND_H_

#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <iostream>
#include <unordered_map>

#include "base/types.h"
#include "tcp/tcp.h"

/// @brief the message format between the proxy client and proxy server.
/// |ProxyMessageHeader|data|
struct ProxyMessageHeader {
  uint32_t src_ip;
  uint16_t src_port;
  uint32_t dst_ip;
  uint16_t dst_port;
  uint16_t len;
#if __BYTE_ORDER == __LITTLE_ENDIAN
  uint16_t type : 2;
  uint16_t resv : 14;
#elif __BYTE_ORDER == __BIG_ENDIAN
  uint16_t resv : 14;
  uint16_t type : 2;
#else
#error \
    "Unsupported byte order: Either __LITTLE_ENDIAN or " \
               "__BIG_ENDIAN must be defined"
#endif  // byte order
};

/// @brief To handle the dl/up message between proxy client and server.
class ProxyHandler {
 public:
  virtual ~ProxyHandler() {}
  virtual bool HandleDlMessage() = 0;
  virtual bool HandleUlMessage() = 0;
  virtual void SetDlChannel(const ChannelPtr& dl_chn) = 0;
  virtual void SetUlChannel(const ChannelPtr& dl_chn) = 0;
  virtual int HandleNewConnection(int fd) = 0;
};

using ProxyHandlerPtr = std::shared_ptr<ProxyHandler>;

class TcpProxyHandlerAdapter : public ProxyHandler {
 public:
  /// @brief To handle the downlink data receiving.
  bool HandleDlMessage() override {
    auto packet = ul_chn_->Read();
    auto conn_info = Parse(packet);
    switch (conn_info.type) {
      case 0:
        HandleDataMessage(packet, conn_info);
        break;
      case 1:
        HandleControlMessage(packet, conn_info);
        break;
      default:
        break;
    }
  }
  /// @brief To handle the uplink data transmitting.
  bool HandleUlMessage() override {
    auto packet = dl_chn_->Read();
    auto proxy_packet = Build(packet);
    ul_chn_->Write(proxy_packet);
  }
  int HandleNewConnection(int fd) override {
    // dl_chn_ ??
    return 0;
  }

 private:
  auto Parse(const PacketPtr& packet) -> TcpConnection {
    return TcpConnection();
  }
  auto Build(const PacketPtr& packet) -> PacketPtr { return nullptr; }
  /// @brief To handle the downlink data receiving.
  /// @param packet
  /// @param info
  void HandleDataMessage(const PacketPtr& packet, const TcpConnection& info) {
    // dl_chn_->Write(packet, conn_info);
  }
  /// @brief To control the management of connections.
  /// @param packet
  /// @param info
  void HandleControlMessage(const PacketPtr& packet,
                            const TcpConnection& info) {
    // dl_chn_->Write(packet, conn_info);
  }
  ChannelPtr dl_chn_ = nullptr;  // downlink channel
  ChannelPtr ul_chn_ = nullptr;  // uplink channel
};
using TcpProxyHandlerAdapterPtr = std::shared_ptr<TcpProxyHandlerAdapter>;

class ProxyBackend {
 public:
  static constexpr int MAX_EVENTS = 1024;
  ProxyBackend() = default;
  ~ProxyBackend() {}

  void Start() {
    server_fd_ = CreateTcpServer(10086);
    Register(server_fd_);
  }
  void SetProxyHandler(const ProxyHandlerPtr& proxy_handler) {
    proxy_handler_ = proxy_handler;
    // add carrier to poller?
    // Register(carrier_->GetFd());
  }

 private:
  bool Register(int fd) {
    fcntl(fd, F_SETFL, O_NONBLOCK);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;  // Requests edge-triggered notification
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
      return false;
    }
    return true;
  }

  bool unRegister(int fd) {
    // unregister event
    struct epoll_event ev;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev) == -1) {
      std::cout << strerror(errno) << std::endl;
      return false;
    }
    return true;
  }

  void EpollThread() {
    while (!is_stop_) {
      // handle read event
      int nfds = epoll_wait(epoll_fd_, events_, MAX_EVENTS, 1 * 100);
      if (nfds == -1) {
        std::cout << "epoll_wait " << strerror(errno);
        continue;
      }
      if (nfds == 0) {
        continue;
      }
      for (int n = 0; n < nfds; ++n) {
        int fd = events_[n].data.fd;
        if (server_fd_ == fd) {
          int fd = proxy_handler_->HandleNewConnection(fd);
          if (fd <= 0) {
            std::cout << "AcceptConnection failed" << std::endl;
            continue;
          }
          Register(fd);
          continue;
        } else if (fd == carrier_fd_) {
          proxy_handler_->HandleDlMessage();
        } else {
          proxy_handler_->HandleUlMessage();
        }
      }
    }
    is_stop_ = true;
  }

  struct epoll_event events_[MAX_EVENTS];
  bool is_stop_ = false;
  int epoll_fd_ = 0;
  int server_fd_ = 0;   // TODO renaming
  int carrier_fd_ = 0;  // TODO renaming
  ProxyHandlerPtr proxy_handler_ = nullptr;
};

#endif  // SRC_TCP_PROXY_BACKEND_H_
