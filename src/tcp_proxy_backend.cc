#include "tcp_proxy_backend.h"

#include "proxy_tunnel.h"
#include "tcp/tcp.h"

void ProxyBackend::Start() {
  tunnel_entity_ = std::make_shared<ProxyTunnel>();
  tcp_entity_ = std::make_shared<TcpConnMgr>();

  tunnel_entity_->SetRxNotifier(tcp_entity_);
  tcp_entity_->SetRxNotifier(tunnel_entity_);
  // set register function of poller
  tunnel_entity_->RegisterToPoller(
      [this](int fd) -> bool { return Register(fd); });
  tcp_entity_->RegisterToPoller(
      [this](int fd) -> bool { return Register(fd); });
}

bool ProxyBackend::Register(int fd) {
  fcntl(fd, F_SETFL, O_NONBLOCK);
  struct epoll_event ev;
  // Requests edge-triggered notification
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = fd;
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
    return false;
  }
  return true;
}

bool ProxyBackend::Unregister(int fd) {
  struct epoll_event ev;
  ev.data.fd = fd;
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev) == -1) {
    std::cout << strerror(errno) << std::endl;
    return false;
  }
  return true;
}

void ProxyBackend::EpollThread() {
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
        auto tcp_mgr = std::dynamic_pointer_cast<TcpConnMgr>(tcp_entity_);
        int accept_fd = tcp_mgr->AcceptConnection(fd);
        if (accept_fd <= 0) {
          std::cout << "AcceptConnection failed" << std::endl;
          continue;
        }
      } else if (fd == tunnel_fd_) {
        tunnel_entity_->ReceiveMessage(fd);
      } else {
        // handle tcp receive
        tcp_entity_->ReceiveMessage(fd);
      }
    }
  }
  is_stop_ = true;
}
