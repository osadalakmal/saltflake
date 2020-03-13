#include "pipe.h"
#include <unistd.h>
#include <iostream>

namespace saltflake {
namespace io {

Pipe::Pipe(uv_loop_t* loop, const std::string& pipeName) {
    m_pipeName = std::string(pipeName);
    m_pipe = new uv_pipe_t();
    ::uv_pipe_init(loop, m_pipe, false);
}

void Pipe::bind() {
    int r;
    unlink(m_pipeName.c_str());
    if ((r = uv_pipe_bind(m_pipe, m_pipeName.c_str()))) {
        std::cerr << "Error binding pipe to " << m_pipeName << "[" << uv_err_name(r) << "]\n";
        throw std::runtime_error("Bind error");
    }
}

Pipe::~Pipe() {
    if (m_pipe) {
        uv_close(reinterpret_cast<uv_handle_t*>(m_pipe), &Pipe::onCloseCb);
        m_pipe = nullptr;
    }
}

void Pipe::onCloseCb(uv_handle_t* handle) {
    if (handle) {
        delete (reinterpret_cast<uv_pipe_t*>(handle));
    }
}

void*& Pipe::data() {
    return m_pipe->data;
}
void Pipe::close() {
    uv_close(reinterpret_cast<uv_handle_t*>(&m_pipe), &Pipe::onCloseCb);
}

}
}
