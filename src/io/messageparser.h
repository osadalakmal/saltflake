#ifndef SALTFLAKE_MESSAGEPARSER_H
#define SALTFLAKE_MESSAGEPARSER_H

#include "../tcb_span.h"

namespace saltflake {
namespace io {

class MessageParser {
public:
    enum class OP_CODE {
        ERROR = 0,
        GET = 1,
        EXIT = 2
    };
    OP_CODE getOpCodeFromBuf(const tcb::span<char>& buf);
};

} // namespace io
} // namespace saltflake

#endif  // SALTFLAKE_MESSAGEPARSER_H
