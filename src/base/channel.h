#ifndef SRC_BASE_CHANNEL_H_
#define SRC_BASE_CHANNEL_H_

#include "packet.h"
#include "types.h"

class Channel {
 public:
  Channel() = default;
  virtual ~Channel() = default;
  virtual PacketPtr Read() = 0;
  virtual bool Write(const PacketPtr& packet) = 0;
  virtual int GetFd() const = 0;
};

using ChannelPtr = std::shared_ptr<Channel>;

#endif  // SRC_BASE_CHANNEL_H_