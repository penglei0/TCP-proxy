#include "tcp_proxy_frontend.h"

ProxyFrontend::~ProxyFrontend() {
  is_stop_ = true;
  for (auto& thread : threads_) {
    thread.join();
  }
}

void ProxyFrontend::Start() {
  rx_tun_ = std::make_shared<TunDevice>("tun0");
  tx_tun_ = std::make_shared<TunDevice>("tun1");
  threads_.push_back(std::thread(&ProxyFrontend::UlThread, this));
  threads_.push_back(std::thread(&ProxyFrontend::DlThread, this));
}

// Ul: uplink (to internet/server)
void ProxyFrontend::UlThread() {
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
void ProxyFrontend::DlThread() {
  while (is_stop_) {
    // read from rx_tun_
    auto packet = tx_tun_->Read();
    // TCP restore
    RestoreTcpPacket(packet);
    // write to tx_tun_
    rx_tun_->Write(packet);
  }
}

void ProxyFrontend::HijackTcpPacket(const PacketPtr& tcp_packet) {}
void ProxyFrontend::RestoreTcpPacket(const PacketPtr& tcp_packet) {}
