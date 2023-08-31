/**
 * @file tcp_proxy_frontend.h
 * @author Peng lei (peng.lei@n-hop.com)
 * @brief The frontend of TCP proxy module is used to redirect the tcp traffic
 * to a local tcp server.
 * @version 0.1
 * @date 2023-08-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef SRC_TCP_PROXY_FRONTEND_H_
#define SRC_TCP_PROXY_FRONTEND_H_

#include <thread>
#include <vector>

#include "tun/tun.h"

class ProxyFrontend {
 public:
  ProxyFrontend() = default;
  ~ProxyFrontend() {
    is_stop_ = true;
    for (auto& thread : threads_) {
      thread.join();
    }
  }

  void Start() {
    rx_tun_ = std::make_shared<TunDevice>("tun0");
    tx_tun_ = std::make_shared<TunDevice>("tun1");
    threads_.push_back(std::thread(&ProxyFrontend::UlThread, this));
    threads_.push_back(std::thread(&ProxyFrontend::DlThread, this));
  }

  // Ul: uplink (to internet/server)
  void UlThread() {
    while (is_stop_) {
      // read from rx_tun_
      auto packet = rx_tun_->Read();
      // TCP hijack
      HijackTcpPacket(packet);
      // write to tx_tun_
      tx_tun_->Write(packet);
    }
  }
  // Dl: downlink (to client/users)
  void DlThread() {
    while (is_stop_) {
      // read from rx_tun_
      auto packet = tx_tun_->Read();
      // TCP restore
      RestoreTcpPacket(packet);
      // write to tx_tun_
      rx_tun_->Write(packet);
    }
  }
  void HijackTcpPacket(const PacketPtr& tcp_packet) {}
  void RestoreTcpPacket(const PacketPtr& tcp_packet) {}

 private:
  bool is_stop_ = false;
  std::vector<std::thread> threads_;
  TunDevicePtr rx_tun_ = nullptr;
  TunDevicePtr tx_tun_ = nullptr;
};

#endif  // SRC_TCP_PROXY_FRONTEND_H_
