#ifndef SALTFLAKE_PIPE_H
#define SALTFLAKE_PIPE_H

#include <uv.h>
#include <memory>

namespace saltflake {
namespace io {

class Pipe {

public:
    Pipe() = default;
    Pipe(uv_loop_t* loop, const std::string& pipeName);
    virtual ~Pipe();

    static void onCloseCb(uv_handle_t* handle);

    uv_pipe_t* get() { return m_pipe; }
    void bind();
    void*& data();
    void close();
private:
    uv_pipe_t* m_pipe;
    std::string m_pipeName;
    Pipe bind() const;
};

}
}

#endif  // SALTFLAKE_PIPE_H
