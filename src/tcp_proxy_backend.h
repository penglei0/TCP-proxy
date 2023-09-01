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

/// @brief The backend of TCP proxy module is used to receive the TCP payload
/// from the frontend, and transmit the TCP payload to the remote proxy
/// backend by using UDP/KCP or other protocols.
class ProxyBackend {
 public:
  ProxyBackend() = default;
  virtual ~ProxyBackend() = default;
  ProxyBackend(const ProxyBackend&) = delete;
  ProxyBackend& operator=(const ProxyBackend&) = delete;
  ProxyBackend(const ProxyBackend&&) = delete;
  ProxyBackend& operator=(const ProxyBackend&&) = delete;
  /// @brief Start the backend.
  void Start();

 private:
  /// @brief Register the file descriptor to epoll.
  /// @param fd The file descriptor to be registered.
  /// @return
  bool Register(int fd);
  /// @brief Unregister the file descriptor to epoll.
  /// @param fd The file descriptor to be registered.
  /// @return
  bool Unregister(int fd);
  /// @brief The thread to handle the epoll events.
  void EpollThread();

  static constexpr int MAX_EVENTS = 1024;
  struct epoll_event events_[MAX_EVENTS];
  bool is_stop_ = false;
  int epoll_fd_ = 0;
  int server_fd_ = 0;  // TODO renaming
  int tunnel_fd_ = 0;  // TODO renaming
  // The rx interface to receive the tcp payload from the proxy tunnel.
  RxTxEntityTcpPtr tunnel_entity_ = nullptr;
  // The rx interface to receive the tcp payload from the remote proxy backend.
  RxTxEntityTcpPtr tcp_entity_ = nullptr;
};

#endif  // SRC_TCP_PROXY_BACKEND_H_
