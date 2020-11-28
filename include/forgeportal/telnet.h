//
// Created by volund on 11/27/20.
//

#ifndef FORGEMUSH_TELNET_H
#define FORGEMUSH_TELNET_H

#include <cstdint>
#include "forgenet/net.h"

namespace forgeportal::telnet {

    enum TelnetCode : uint8_t {
        NULL = 0,
        BEL = 7,
        CR = 13,
        LF = 10,
        SGA = 3,
        TELOPT_EOR = 25,
        NAWS = 31,
        LINEMODE = 34,
        EOR = 239,
        SE = 240,
        NOP = 241,
        GA = 249,
        SB = 250,
        WILL = 251,
        WONT = 252,
        DO = 253,
        DONT = 254,
        IAC = 255,

        // MNES: Mud New-Environ Standard
        MNES = 39,

        // MXP: MUD eXtension Protocol
        MXP = 91,

        // MSSP: Mud Server Status Protocol
        MSSP = 70,

        // MCCP#: Mud Client Compression Protocol
        MCCP2 = 86,
        MCCP3 = 87,

        // GMCP: Generic Mud Communication Protocol
        GMCP = 201,

        // MSDP: Mud Server Data Protocol
        MSDP = 69,

        // TTYPE - Terminal Type
        TTYPE = 24

    };

    class TelnetProtocol : forgeportal::net::Protocol {

    };

}

#endif //FORGEMUSH_TELNET_H
