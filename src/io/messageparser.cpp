#include "messageparser.h"

namespace saltflake {
namespace io {

MessageParser::OP_CODE MessageParser::getOpCodeFromBuf(
    const tcb::span<char>& buf) {
    if (buf.size() < 2) {
        return OP_CODE::ERROR;
    }
    switch (buf[1]) {
        case 'g':
            return OP_CODE::GET;
        case 'x':
            return OP_CODE::EXIT;
        default:
            return OP_CODE::ERROR;
    }
}

}  // namespace io
}  // namespace saltflake
