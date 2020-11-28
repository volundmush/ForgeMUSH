//
// Created by volund on 11/27/20.
//

#ifndef FORGENET_PROTOCOL_H
#define FORGENET_PROTOCOL_H

#include <string>
#include <unordered_map>
#include <cstdint>
#include <boost/asio.hpp>

namespace forgenet::net {

    class Manager;
    class Server;
    class Protocol;


    class Protocol {
    public:
        Protocol(Server *server, std::string id, boost::asio::ip::tcp::socket&& socket);
        virtual const char* getName() = 0;
        const char* getId();
        virtual void onReceiveData(std::vector<uint8_t> data, size_t length) = 0;
        virtual void onClose() = 0;
        virtual void onLost() = 0;
        virtual void onConnect() = 0;
    private:
        std::string id;
        boost::asio::io_context *context;
        Manager *manager;
        Server *server;
        boost::asio::ip::tcp::socket socket;
    };

    typedef void (*make_protocol_cb)(Server*, boost::asio::ip::tcp::socket*);

    class Server {
    public:
        Server(Manager *manager, std::string name, std::string protocol_name, make_protocol_cb prot_cb);
        Manager *manager;
        boost::asio::io_context *context;
        const char* getName();
    private:
        std::string name, protocol_name;
        make_protocol_cb prot_cb;
        boost::asio::ip::tcp::acceptor acceptor;

    };

    class Manager {
    public:
    private:
        std::unordered_map<std::string, Server> servers;
        std::unordered_map<std::string, Protocol*> protocols;
        boost::asio::io_context *context;
    };
}


#endif //FORGENET_PROTOCOL_H
