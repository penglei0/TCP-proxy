#ifndef SRC_TUN_TUN_H_
#define SRC_TUN_TUN_H_

#include <memory>
#include <string>

#include "base/packet.h"

class TunDevice {
 public:
  explicit TunDevice(const std::string& name) : name_(name) {}
  virtual ~TunDevice() = default;
  // disable copy and move
  TunDevice(const TunDevice&) = delete;
  TunDevice& operator=(const TunDevice&) = delete;
  TunDevice(const TunDevice&&) = delete;
  TunDevice& operator=(const TunDevice&&) = delete;

  bool Open() { return false; }
  bool Close() { return false; }
  PacketPtr Read() { return nullptr; }
  bool Write(const PacketPtr& packet) { return false; }
  int GetFd() const { return fd_; }

 private:
  int fd_;
  std::string name_;
};

using TunDevicePtr = std::shared_ptr<TunDevice>;

#endif  // SRC_TUN_TUN_H_
