#include "tcp.h"

int CreateTcpServer(int port) { return 0; }

TcpConnMgr::TcpConnMgr() { server_fd_ = CreateTcpServer(10086); }
TcpConnMgr::~TcpConnMgr() {}

int TcpConnMgr::AcceptConnection(int fd) { return 0; }
bool TcpConnMgr::CloseConnection(const TcpConnection& info) { return false; }
auto TcpConnMgr::GetConnection(uint64_t key) -> TcpConnection {
  return TcpConnection();
}
bool TcpConnMgr::ReceiveMessage(int fd) {
  // TODO find the tcp channel by fd.
  /*
  TCPChannelPtr tcp;
  auto packet = tcp->ReadUntil();
  if (packet) {
    const auto& conn = tcp->ConnectionInfo();
    notifier_->OnNewMessage(packet, conn);
  }
  */
  return false;
}
bool TcpConnMgr::SendMessage(const PacketPtr& packet,
                             const TcpConnection& connectionInfo) {
  return false;
}
/// @brief Other modules will call this function to notify the reception of
/// new packets.
bool TcpConnMgr::OnNewMessage(const PacketPtr& packet,
                              const TcpConnection& info) {
  return SendMessage(packet, info);
}

void TcpConnMgr::SetRxNotifier(const RxNotifierPtr& notifier) {
  notifier_ = notifier;
}
