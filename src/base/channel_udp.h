#ifndef SRC_BASE_CHANNEL_UDP_H_
#define SRC_BASE_CHANNEL_UDP_H_

#include <string>

#include "base/channel.h"

class UdpChannelCfg {
 public:
  int port = 0;
  std::string ip;
};

class UDPChannel : public Channel {
 public:
  UDPChannel() = default;
  explicit UDPChannel(const UdpChannelCfg& cfg) {}
  virtual ~UDPChannel() = default;
  UDPChannel(const UDPChannel&) = delete;
  UDPChannel& operator=(const UDPChannel&) = delete;
  UDPChannel(const UDPChannel&&) = delete;
  UDPChannel& operator=(const UDPChannel&&) = delete;

  PacketPtr Read() override { return nullptr; }
  bool Write(const PacketPtr& packet) override { return true; }
  int GetFd() const override { return fd_; }

 private:
  int fd_ = 0;
};

using UDPChannelPtr = std::shared_ptr<UDPChannel>;

#endif  // SRC_BASE_CHANNEL_UDP_H_
