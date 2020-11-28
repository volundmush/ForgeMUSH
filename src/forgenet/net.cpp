//
// Created by volund on 11/27/20.
//

#include "forgenet/net.h"
namespace forgenet::net {

    Protocol::Protocol(Server *server, std::string id, boost::asio::ip::tcp::socket&& socket) : socket(std::move(socket)) {
        this->server = server;
        manager = server->manager;
        context = server->context;
    }

}