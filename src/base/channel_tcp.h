#ifndef SRC_BASE_CHANNEL_TCP_H_
#define SRC_BASE_CHANNEL_TCP_H_

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "channel.h"

class TcpChannelCfg {
 public:
  int port = 0;
  std::string ip;
};

class TCPChannel : public Channel {
 public:
  TCPChannel() = default;
  explicit TCPChannel(const TcpChannelCfg& cfg) {}
  virtual ~TCPChannel() = default;
  TCPChannel(const TCPChannel&) = delete;
  TCPChannel& operator=(const TCPChannel&) = delete;
  TCPChannel(const TCPChannel&&) = delete;
  TCPChannel& operator=(const TCPChannel&&) = delete;
  bool SetNoneBlock() override {
    if (fd_ <= 0) {
      return false;
    }
    if (fcntl(fd_, F_SETFL, O_NONBLOCK) == -1) {
      std::cerr << "set nonblock failed, " << strerror(errno);
      return false;
    }
    return true;
  }
  void ReadUntil(const PacketCallback& packet_callback) override {
    int received = 0;
    octet buf[max_packet_size];
    while (true) {
      int ret = read(fd_, buf + received, max_packet_size - received);
      if (ret < 0 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
        break;
      }
      received += ret;
    }
    // TODO: reduce copy and memory allocation
    auto packet = std::make_shared<Packet>();
    if (received > packet->Capacity()) {
      std::cerr << "packet size is too large: " << received << std::endl;
      return;
    }
    std::copy(buf, buf + received, std::back_inserter(packet->Data()));
    packet_callback(packet);
  }
  PacketPtr ReadOnce() override {
    int received = 0;
    octet buf[max_packet_size];
    int ret = read(fd_, buf, max_packet_size);
    if (ret < 0) {
      // TODO shutdown channel?
      return nullptr;
    }
    auto packet = std::make_shared<Packet>();
    if (ret > packet->Capacity()) {
      std::cerr << "packet size is too large: " << ret << std::endl;
      return nullptr;
    }
    std::copy(buf, buf + ret, std::back_inserter(packet->Data()));
    return packet;
  }
  bool Write(const PacketPtr& packet) override { return true; }
  int GetFd() const override { return fd_; }
  const TcpConnection& ConnectionInfo() const { return conn_info_; }
  TcpConnection& ConnectionInfo() { return conn_info_; }

 private:
  int fd_ = 0;
  TcpConnection conn_info_;
};
using TCPChannelPtr = std::shared_ptr<TCPChannel>;

#endif  // SRC_BASE_CHANNEL_TCP_H_
