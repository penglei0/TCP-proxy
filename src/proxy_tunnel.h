/**
 * @file proxy_tunnel.h
 * @author Peng lei (peng.lei@n-hop.com)
 * @brief The proxy tunnel protocol.
 * @version 0.1
 * @date 2023-08-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef SRC_PROXY_TUNNEL_H_
#define SRC_PROXY_TUNNEL_H_

#include <functional>

#include "base/channel_udp.h"
#include "base/proto.h"
#include "base/rx_tx_interface.h"
#include "tcp/tcp.h"

/// @brief the message format between the proxy client and proxy server.
/// | ProxyMessageHeader | data |
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

// Decorators
class ProxyProtocol : public UDPChannel, public ProtoInterface<TcpConnection> {
 public:
  explicit ProxyProtocol(const UDPChannelPtr& chn) : chn_(chn) {}
  int GetFd() const override { return chn_->GetFd(); }
  PacketPtr Read() override { return chn_->Read(); }
  bool Write(const PacketPtr& packet) override { return chn_->Write(packet); }
  TcpConnection Parse(const PacketPtr& packet) override {
    // TODO: parse the packet to TcpConnection.
  }
  PacketPtr Build(const PacketPtr& packet) override {
    // TODO:
  }

 private:
  UDPChannelPtr chn_ = nullptr;
};

using ProxyProtocolPtr = std::shared_ptr<ProxyProtocol>;

class ProxyTunnel : public RxTxInterface<TcpConnection>,
                    public RxNotifier<TcpConnection> {
 public:
  virtual ~ProxyTunnel() {}
  bool SendMessage(const PacketPtr& packet,
                   const TcpConnection& connectionInfo) override {
    // connectionInfo
    if (protocol_) {
      auto proxy_message = protocol_->Build(packet);
      protocol_->Write(proxy_message);
    }
  }
  bool ReceiveMessage(int fd) override {
    auto proxy_message = protocol_->Read();
    auto conn_info = protocol_->Parse(proxy_message);
    switch (conn_info.type) {
      case 0:
        HandleDataMessage(proxy_message, conn_info);
        break;
      case 1:
        HandleControlMessage(proxy_message, conn_info);
        break;
      default:
        break;
    }
  }
  /// @brief Other modules will call this function to notify the reception of
  /// new packets.
  bool OnNewMessage(const PacketPtr& packet,
                    const TcpConnection& info) override {
    return SendMessage(packet, info);
  }
  void SetNotifier(const RxNotifierPtr& notifier) override {
    notifier_ = notifier;
  }

 private:
  /// @brief To handle the downlink data receiving.
  /// @param packet
  /// @param info
  void HandleDataMessage(const PacketPtr& packet, const TcpConnection& info) {
    // data to tcp manager.
    notifier_->OnNewMessage(packet, info);
  }
  /// @brief To control the management of connections.
  /// @param packet
  /// @param info
  void HandleControlMessage(const PacketPtr& packet,
                            const TcpConnection& info) {
    notifier_->OnNewMessage(packet, info);
  }

  ProxyProtocolPtr protocol_ = nullptr;
  RxNotifierPtr notifier_ = nullptr;  // to notify the reception of new packets.
};

using ProxyTunnelPtr = std::shared_ptr<ProxyTunnel>;

#endif  // SRC_PROXY_TUNNEL_H_