#include "proxy_tunnel.h"

TcpConnection ProxyProtocol::Parse(const PacketPtr& packet) {
  // TODO: parse the packet to TcpConnection.
  return TcpConnection();
}
PacketPtr ProxyProtocol::Build(const PacketPtr& packet) {
  // TODO:
  return nullptr;
}

bool ProxyTunnel::SendMessage(const PacketPtr& packet,
                              const TcpConnection& connectionInfo) {
  // connectionInfo
  if (protocol_) {
    auto proxy_message = protocol_->Build(packet);
    protocol_->Write(proxy_message);
  }
  return false;
}

bool ProxyTunnel::ReceiveMessage(int fd) {
  protocol_->ReadUntil([this](const PacketPtr& packet) -> bool {
    auto conn_info = protocol_->Parse(packet);
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
    return true;
  });
  return false;
}

/// @brief Other modules will call this function to notify the reception of
/// new packets.
bool ProxyTunnel::OnNewMessage(const PacketPtr& packet,
                               const TcpConnection& info) {
  return SendMessage(packet, info);
}

void ProxyTunnel::SetRxNotifier(const RxNotifierPtr& notifier) {
  notifier_ = notifier;
}
/// @brief To handle the downlink data receiving.
/// @param packet
/// @param info
void ProxyTunnel::HandleDataMessage(const PacketPtr& packet,
                                    const TcpConnection& info) {
  // data to tcp manager.
  notifier_->OnNewMessage(packet, info);
}
/// @brief To control the management of connections.
/// @param packet
/// @param info
void ProxyTunnel::HandleControlMessage(const PacketPtr& packet,
                                       const TcpConnection& info) {
  notifier_->OnNewMessage(packet, info);
}
