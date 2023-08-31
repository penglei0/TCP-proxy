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
#include "proxy_tunnel.h"
#include "tcp/tcp.h"

class ProxyBackend {
 public:
  static constexpr int MAX_EVENTS = 1024;
  ProxyBackend() {}
  virtual ~ProxyBackend() {}

  void Start() {
    tunnel_ = std::make_shared<ProxyTunnel>();
    tcp_ = std::make_shared<TcpConnMgr>();
    // TODO: set the notifier.
    auto tcp_notifier =
        std::dynamic_pointer_cast<RxNotifier<TcpConnection>>(tcp_);
    tunnel_->SetNotifier(tcp_notifier);

    auto tunnel_notifier =
        std::dynamic_pointer_cast<RxNotifier<TcpConnection>>(tunnel_);
    tcp_->SetNotifier(tunnel_notifier);

    Register(server_fd_);
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
          auto tcp_mgr = std::dynamic_pointer_cast<TcpConnMgr>(tcp_);
          int accept_fd = tcp_mgr->AcceptConnection(fd);
          if (accept_fd <= 0) {
            std::cout << "AcceptConnection failed" << std::endl;
            continue;
          }
          Register(accept_fd);
        } else if (fd == tunnel_fd_) {
          tunnel_->ReceiveMessage(fd);
        } else {
          // handle tcp receive
          tcp_->ReceiveMessage(fd);
        }
      }
    }
    is_stop_ = true;
  }

  struct epoll_event events_[MAX_EVENTS];
  bool is_stop_ = false;
  int epoll_fd_ = 0;
  int server_fd_ = 0;  // TODO renaming
  int tunnel_fd_ = 0;  // TODO renaming
  RxTxInterfacePtr tunnel_ = nullptr;
  RxTxInterfacePtr tcp_ = nullptr;
};

#endif  // SRC_TCP_PROXY_BACKEND_H_
