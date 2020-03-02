#ifndef SALTFLAKE_TESTCLIENT_H
#define SALTFLAKE_TESTCLIENT_H

#include <sys/socket.h>
#include <sys/un.h>
#include <tcb_span.h>

namespace saltflake {

class TestClient {
public:
    TestClient(const char* name) {
        struct sockaddr_un sock;
        memset(&sock, 0, sizeof(struct sockaddr_un));

        m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
        if (m_socket < 0) {
            perror("opening domain socket");
            exit(1);
        }
        sock.sun_family = AF_UNIX;
        strncpy(sock.sun_path, name, strlen(name));
        if (connect(m_socket, (struct sockaddr *) &sock, sizeof(struct sockaddr_un))) {
            perror("connecting to domain socket");
            exit(1);
        }
    }

    bool sendMessage() {
        char data[2] = {'1', 'g'};
        tcb::span<char> buf(data, 2);
        return (send(m_socket, buf.data(), buf.size(), 0) == 0);
    }

    bool sendExit() {
        char data[2] = {'1', 'x'};
        tcb::span<char> buf(data, 2);
        return (send(m_socket, buf.data(), buf.size(), 0) == 0);
    }

    bool recvMessage(tcb::span<char> buf) {
        ssize_t bytes_read;
        bytes_read = recv(m_socket, buf.begin(), buf.size(), 0);
        if (bytes_read < 0) {
            return false;
        }
        return true;
    }

private:
    unsigned int m_socket;
};

} // namespace saltflake

#endif  // SALTFLAKE_TESTCLIENT_H
