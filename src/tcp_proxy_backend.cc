#include "tcp_proxy_backend.h"

#include "proxy_tunnel.h"

void ProxyBackend::Start() {
  tunnel_rx_ = std::make_shared<ProxyTunnel>();
  tcp_rx_ = std::make_shared<TcpConnMgr>();
  auto tcp_notifier =
      std::dynamic_pointer_cast<RxNotifier<TcpConnection>>(tcp_rx_);
  auto tunnel_notifier =
      std::dynamic_pointer_cast<RxNotifier<TcpConnection>>(tunnel_rx_);
  tunnel_rx_->SetRxNotifier(tcp_notifier);
  tcp_rx_->SetRxNotifier(tunnel_notifier);
  Register(server_fd_);
}

bool ProxyBackend::Register(int fd) {
  fcntl(fd, F_SETFL, O_NONBLOCK);
  struct epoll_event ev;
  ev.events = EPOLLIN | EPOLLET;  // Requests edge-triggered notification
  ev.data.fd = fd;
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
    return false;
  }
  return true;
}

bool ProxyBackend::Unregister(int fd) {
  // unregister event
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
        auto tcp_mgr = std::dynamic_pointer_cast<TcpConnMgr>(tcp_rx_);
        int accept_fd = tcp_mgr->AcceptConnection(fd);
        if (accept_fd <= 0) {
          std::cout << "AcceptConnection failed" << std::endl;
          continue;
        }
        Register(accept_fd);
      } else if (fd == tunnel_fd_) {
        tunnel_rx_->ReceiveMessage(fd);
      } else {
        // handle tcp receive
        tcp_rx_->ReceiveMessage(fd);
      }
    }
  }
  is_stop_ = true;
}
