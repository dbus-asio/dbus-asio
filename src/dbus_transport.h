#ifndef DBUS_TRANSPORT_H
#define DBUS_TRANSPORT_H

#include <condition_variable>
#include <thread>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <sys/socket.h>
#include <sys/un.h>

#define USE_ASIO_NORMAL 1
#define USE_ASIO_ACCESSOR 0
#define USE_NATIVE_SOCKETS 0

namespace DBus {

typedef std::function<void(uint8_t c)> ReceiveOctetCallbackFunction;

class Transport {
public:
    Transport(const std::string& path);
    ~Transport();

    void sendString(const std::string& data);
    void handle_write_output(std::shared_ptr<std::string> buf_written, const boost::system::error_code& error, std::size_t bytes_transferred);

    void ThreadFunction();

    void setOctetHandler(const ReceiveOctetCallbackFunction& callback);
    void onReceiveOctet(uint8_t c);
    void onAuthComplete();
    void addToMessageQueue(const std::string& data);

    std::string getStats() const;

protected:
    friend class AuthenticationProtocol; // this is to give access to sendStringDirect, to bypass the buffering
    void sendOctetDirect(uint8_t data);
    void sendStringDirect(const std::string& data);

    std::string m_Busname;
    bool m_ReadyToSend;
    bool m_ReadyToReceive;
    boost::asio::io_service m_io_service;
    boost::asio::local::stream_protocol::socket m_socket;

    uint8_t m_DataBuffer[1];
    ReceiveOctetCallbackFunction m_ReceiveOctetCallback;

    std::mutex m_StartUpMutex;
    std::condition_variable m_StartUpCondition;
    mutable boost::recursive_mutex m_SendMutex;
    mutable boost::recursive_mutex m_CallbackMutex;
    std::vector<std::string> m_BufferedMessages;
    volatile bool m_QuitThread;
    std::thread m_Thread;

private:
    struct Stats {
        size_t count_messagessent;
        size_t count_messagesqueued;
        size_t count_messagespumped;
        size_t bytes_sent;
        size_t bytes_read;
    } m_Stats;
};
}

#endif // DBUS_TRANSPORT_H
