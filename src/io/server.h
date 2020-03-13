#ifndef SALTFLAKE_SERVER_H
#define SALTFLAKE_SERVER_H

#include <uv.h>

#include <memory>
#include <string>
#include <vector>

#include "pipe.h"

namespace saltflake {
namespace io {

enum class ResultCode {
    ERR_NONE,
    ERR_IO
};

class Pipe;

class Server : public std::enable_shared_from_this<Server> {
public:
    Server(const std::string& pipeName);
    ResultCode open();
    virtual ~Server();

    static void onNewConnection(uv_stream_t *server, int status);
    const std::string& pipeName() const { return m_pipeName; }
    uv_loop_t* getLoop() { return m_mainLoop; }
    Pipe& addClient(uv_loop_t* loop, const std::string& pipeName);
    int run();
    int close();

private:
    uv_loop_t* m_mainLoop;
    Pipe* m_server;
    std::vector<Pipe*> m_clients;
    std::string m_pipeName;
    uv_async_t* m_async;
};

} // namespace io
} // namespace saltflake

#endif  // SALTFLAKE_SERVER_H
