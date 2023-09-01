#ifndef SRC_BASE_TYPES_H_
#define SRC_BASE_TYPES_H_

#include <stdint.h>

#include <vector>

using octet = uint8_t;
using octetVec = std::vector<octet>;

/// @brief the information of the tcp connection.
class TcpConnection {
 public:
  uint32_t src_ip;
  uint16_t src_port;
  uint32_t dst_ip;
  uint16_t dst_port;
  int fd;
  int type;
};

#endif  // SRC_BASE_TYPES_H_