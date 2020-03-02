#include "server.h"

#include <unistd.h>
#include <uv.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "pipe.h"

namespace saltflake {
namespace io {

namespace {

void alloc_cb(uv_handle_t*, size_t size, uv_buf_t* buf) {
    buf->base = reinterpret_cast<char*>(malloc(size));
    buf->len = size;
}

void write_cb(uv_write_t* req, int status) {
    if (status < 0) {
        fprintf(stderr, "write error: [%s: %s]\n", uv_err_name((status)), uv_strerror((status)));
        assert(0);
    }
    char *base = (char*) req->data;
    free(base);
    delete req;
}

void read_cb(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
    if (nread < 0) {
        fprintf(stderr, "read error: [%s: %s]\n", uv_err_name((nread)),
                uv_strerror((nread)));
        uv_close(reinterpret_cast<uv_handle_t*>(client), nullptr);
        return;
    }

    auto req = new uv_write_t();
    req->data = (void*) buf->base;

    uv_write(req, client, buf, 1, write_cb);
}

void on_uv_close(uv_handle_t* handle)
{
    if (handle != nullptr && handle->type != UV_ASYNC) {
        delete handle;
    }
}

void on_uv_walk(uv_handle_t* handle, void*) {
    uv_close(handle, on_uv_close);
}

void async_cb(uv_async_t* t) {
    uv_stop(reinterpret_cast<uv_loop_t*>(t->data));
}

} // anonymous namespace

Server::Server(const std::string& pipeName) : m_pipeName(pipeName) {
    m_mainLoop = new uv_loop_t();
    uv_loop_init(m_mainLoop);
    m_async = new uv_async_t();
    uv_async_init(m_mainLoop, m_async, async_cb);
    m_async->data = m_mainLoop;
    m_server = new Pipe(m_mainLoop, pipeName);
    m_server->bind();
    m_server->data() = this;
    if (auto r = uv_listen((uv_stream_t*)m_server->get(), 128,
                           &Server::onNewConnection)) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(r));
        throw std::runtime_error("Listen error");
    }
}

Server::~Server() {
    std::for_each(m_clients.begin(), m_clients.end(),
                  [](Pipe* pipe) { delete pipe; });
    delete m_server;
    uv_close(reinterpret_cast<uv_handle_t*>(m_async), on_uv_close);
    uv_walk(
        m_mainLoop,
        [](uv_handle_t* handle, void*) {
            if (!uv_is_closing(handle))
                uv_close(handle, nullptr);
        },
        nullptr);
    uv_run(m_mainLoop, UV_RUN_DEFAULT);
    assert(UV_EBUSY != uv_loop_close(m_mainLoop));
    delete m_mainLoop;
    delete m_async;
}

void Server::onNewConnection(uv_stream_t *server, int status) {
    if (status) {
        fprintf(stderr, "connection error: [%s: %s]\n", uv_err_name((status)), uv_strerror((status)));
        return;
    }

    auto* s = reinterpret_cast<Server*>(server->data);
    Pipe& pipe = s->addClient(s->getLoop(), s->pipeName());

    int r;
    if (r = uv_pipe_init(s->getLoop(), pipe.get(), 0)) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(r));
        throw std::runtime_error("Listen error");
    }

    r = uv_accept(server, reinterpret_cast<uv_stream_t*>(pipe.get()));
    if (r == 0) {
        uv_read_start(reinterpret_cast<uv_stream_t*>(pipe.get()), alloc_cb, read_cb);
    } else {
        uv_close((uv_handle_t*)pipe.get(), nullptr);
    }
}

Pipe& Server::addClient(uv_loop_t* loop, const std::string& pipeName) {
    m_clients.push_back(new Pipe(loop, pipeName));
    return *(m_clients.back());
}

int Server::run() {
    return uv_run(m_mainLoop, UV_RUN_DEFAULT);
}
int Server::close() {
    uv_async_send(m_async);
}

} // namespace io
} // namespace saltflake
