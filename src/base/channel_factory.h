#ifndef SRC_BASE_CHANNEL_FACTORY_H_
#define SRC_BASE_CHANNEL_FACTORY_H_

#include <string>

#include "base/channel.h"
#include "base/channel_udp.h"

class TunnelChannelFactory {
 public:
  virtual ~TunnelChannelFactory() {}
  ChannelPtr CreateChannel(const std::string &chn_type) {
    if (chn_type == "udp") {
      UdpChannelCfg cfg;
      cfg.port = 8888;
      cfg.ip = "127.0.0.1";
      return std::make_shared<UDPChannel>(cfg);
    }
    return nullptr;
  }
};

#endif  // SRC_BASE_CHANNEL_FACTORY_H_
