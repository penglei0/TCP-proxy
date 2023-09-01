#ifndef SRC_BASE_PACKET_H_
#define SRC_BASE_PACKET_H_

#include <stdint.h>

#include <memory>
#include <vector>

#include "types.h"

constexpr int max_packet_size = 1500;
class Packet {
 public:
  Packet() { data_.resize(max_packet_size); }
  explicit Packet(std::vector<octet>&& data) : data_(std::move(data)) {}
  virtual ~Packet() {}
  // disable copy and move
  Packet(const Packet&) = delete;
  Packet& operator=(const Packet&) = delete;
  Packet(const Packet&&) = delete;
  Packet& operator=(const Packet&&) = delete;
  std::vector<octet>& Data() { return data_; }
  const std::vector<octet>& Data() const { return data_; }
  int Capacity() const { return data_.capacity(); }

 private:
  std::vector<octet> data_;
};

using PacketPtr = std::shared_ptr<Packet>;
// using PacketQueue = Queue<PacketPtr>;

#endif  // SRC_BASE_PACKET_H_