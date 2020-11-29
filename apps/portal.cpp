#include <iostream>
#include "forgeportal/net.h"

int main() {
    boost::asio::io_context con;
    forgeportal::net::ConnectionManager cm;
    forgeportal::net::ServerManager sm(con, cm);

    sm.registerAddress(std::string("mine"), std::string("10.0.0.226"));

    std::optional<std::string> ssl;
    sm.createServer(std::string("testing"), std::string("mine"), 7999, std::string("telnet"), ssl);
    sm.start();

    con.run();
    return 0;
}
