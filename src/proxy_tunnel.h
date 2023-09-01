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

#include "base/channel_tcp.h"
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

/// @brief A proxy protocol based on UDP channel and ProtoInterface of
/// TcpConnection. When we say "proxy protocol", it is a combination of message
/// formatting and its corresponding transmission protocol.
class ProxyProtocol : public UDPChannel, public ProtoInterface<TcpConnection> {
 public:
  explicit ProxyProtocol(const UDPChannelPtr& chn) : chn_(chn) {}
  ProxyProtocol(const ProxyProtocol&) = delete;
  ProxyProtocol& operator=(const ProxyProtocol&) = delete;
  ProxyProtocol(const ProxyProtocol&&) = delete;
  ProxyProtocol& operator=(const ProxyProtocol&&) = delete;
  /// @brief Get the file descriptor of the channel.
  /// @return
  int GetFd() const override { return chn_->GetFd(); }
  /// @brief Read the packet from the channel.
  /// @return The packet read from the channel.
  PacketPtr ReadUntil() override { return chn_->ReadUntil(); }
  /// @brief Write the packet to the channel.
  /// @param packet The packet to be written.
  bool Write(const PacketPtr& packet) override { return chn_->Write(packet); }

  /// @brief Parse the packet to TcpConnection. Call this when receiving the
  /// packet from the proxy tunnel.
  /// @param packet The received packet from the proxy tunnel.
  /// @return
  TcpConnection Parse(const PacketPtr& packet) override;
  /// @brief Build a proxy tunnel format packet from the input packet. Call this
  /// when sending the packet to the proxy tunnel.
  /// @param packet The packet to be sent to the proxy tunnel.
  /// @return The packet in proxy tunnel format.
  PacketPtr Build(const PacketPtr& packet) override;

 private:
  /// @brief The underlying channel to tx/rx packets to/from the proxy tunnel.
  UDPChannelPtr chn_ = nullptr;
};
using ProxyProtocolPtr = std::shared_ptr<ProxyProtocol>;

/// @brief The interface of transmitting/receiving packets to/from the tunnel.
class ProxyTunnel : public RxInterface<TcpConnection>,
                    public TxInterface<TcpConnection>,
                    public RxNotifier<TcpConnection> {
 public:
  ProxyTunnel() = default;
  virtual ~ProxyTunnel() {}
  // disable copy and move
  ProxyTunnel(const ProxyTunnel&) = delete;
  ProxyTunnel& operator=(const ProxyTunnel&) = delete;
  ProxyTunnel(const ProxyTunnel&&) = delete;
  ProxyTunnel& operator=(const ProxyTunnel&&) = delete;
  /// @brief receive the packet from the proxy tunnel.
  /// @param fd The file descriptor of the proxy tunnel channel.
  /// @return
  bool ReceiveMessage(int fd) override;
  /// @brief send the packet to the proxy tunnel.
  /// @param packet The packet to be sent.
  /// @param connectionInfo The connection information of the packet.
  /// @return
  bool SendMessage(const PacketPtr& packet,
                   const TcpConnection& connectionInfo) override;
  /// @brief Other modules will call this function to notify the reception of
  /// new packets.
  /// @param packet The received packet.
  /// @param info The connection information of the packet.
  /// @return
  bool OnNewMessage(const PacketPtr& packet,
                    const TcpConnection& info) override;
  /// @brief Set the rx notifier. The newly received packets will be notified to
  /// the rx notifier.
  void SetRxNotifier(const RxNotifierPtr& notifier) override;

 private:
  /// @brief To handle data message.
  /// @param packet
  /// @param info
  void HandleDataMessage(const PacketPtr& packet, const TcpConnection& info);
  /// @brief To handle control message.
  /// @param packet
  /// @param info
  void HandleControlMessage(const PacketPtr& packet, const TcpConnection& info);

  /// The proxy protocol instance.
  ProxyProtocolPtr protocol_ = nullptr;
  // to notify the reception of new packets.
  RxNotifierPtr notifier_ = nullptr;
};
using ProxyTunnelPtr = std::shared_ptr<ProxyTunnel>;

#endif  // SRC_PROXY_TUNNEL_H_
