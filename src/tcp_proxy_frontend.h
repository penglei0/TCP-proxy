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

/// @brief The frontend of TCP proxy module is used to redirect the tcp traffic
/// to a local tcp server. The backend of the TCP proxy module plays the role of
/// the tcp server.
class ProxyFrontend {
 public:
  ProxyFrontend() = default;
  virtual ~ProxyFrontend();
  /// @brief Start the frontend.
  void Start();
  /// @brief Ul: uplink (to the proxy backend)
  void UlThread();
  /// @brief Dl: downlink (from the proxy backend)
  void DlThread();
  /// @brief Hijack the tcp packet. This function will replace the destination
  /// ip address and destination port in the tcp packet.
  /// @param tcp_packet The tcp packet to be hijacked.
  void HijackTcpPacket(const PacketPtr& tcp_packet);
  /// @brief Restore the tcp packet. This function will restore the source ip
  /// and source port to the original value.
  /// @param tcp_packet
  void RestoreTcpPacket(const PacketPtr& tcp_packet);

 private:
  bool is_stop_ = false;
  // The threads handle the uplink and downlink traffic.
  std::vector<std::thread> threads_;
  // The tun device which used to receive the original tcp traffic.
  TunDevicePtr rx_tun_ = nullptr;
  // The tun device which used to send the hijacked tcp traffic.
  TunDevicePtr tx_tun_ = nullptr;
};

#endif  // SRC_TCP_PROXY_FRONTEND_H_
