#ifndef SRC_BASE_CHANNEL_H_
#define SRC_BASE_CHANNEL_H_

#include <functional>

#include "packet.h"
#include "types.h"

using PacketCallback = std::function<bool(const PacketPtr& packet)>;

class Channel {
 public:
  Channel() = default;
  virtual ~Channel() = default;
  virtual void ReadUntil(const PacketCallback& packet_callback) = 0;
  virtual PacketPtr ReadOnce() = 0;
  virtual bool Write(const PacketPtr& packet) = 0;
  virtual int GetFd() const = 0;
  virtual bool SetNoneBlock() = 0;
};

using ChannelPtr = std::shared_ptr<Channel>;

#endif  // SRC_BASE_CHANNEL_H_