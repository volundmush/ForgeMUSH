//
// Created by volund on 11/28/20.
//

#include "forgeportal/telnet.h"

namespace forgeportal::telnet {

    void TelnetCodec::onReceiveData() {

        while(conn->inbox.size() > 0) {
            auto box = conn->inbox.data();
            auto available = conn->inbox.size();
            auto begin = boost::asio::buffers_begin(box), end = boost::asio::buffers_end(box);
            // first, we read ahead
            if((uint8_t)*begin == (uint8_t)TelnetCode::IAC) {
                if(available < 2) {
                    // not enough bytes available - do nothing;
                    return;
                } else {
                    // we have 2 or more bytes!
                    auto b = begin;
                    b++;
                    if((uint8_t)*b == (uint8_t)TelnetCode::IAC) {
                        // this is an escaped IAC.
                        auto t = new TelnetMessage;
                        t->msg_type = TelnetMsgType::AppData;
                        t->data.push_back(TelnetCode::IAC);
                        conn->prot->receiveFromCodec(t);
                        conn->inbox.consume(2);
                        continue;
                    } else {
                        // It's not an escaped IAC...
                        if(((uint8_t)*b == (uint8_t)TelnetCode::WILL) ||
                            ((uint8_t)*b == (uint8_t)TelnetCode::WONT) ||
                            ((uint8_t)*b == (uint8_t)TelnetCode::DO) ||
                            ((uint8_t)*b == (uint8_t)TelnetCode::DONT)) {
                            if(conn->inbox.size() > 2) {
                                // IAC negotiation received.
                                auto t = new TelnetMessage;
                                t->msg_type = TelnetMsgType::Negotiation;
                                t->codes[0] = (uint8_t)*b;
                                t->codes[1] = (uint8_t)*(++b);
                                conn->prot->receiveFromCodec(t);
                                conn->inbox.consume(3);
                                continue;
                            } else {
                                // It's negotiation, but we need more data.
                                return;
                            }
                        } else {
                            // It's not a negotiation, so it's either a subnegotiation or a command.
                            if((uint8_t)*b == (uint8_t)TelnetCode::SB) {
                                // This is a subnegotiation. we will require at least 5 bytes for this to be usable.
                                if(conn->inbox.size() >= 5) {
                                    uint8_t option = *(++b);
                                    auto sub = ++b;
                                    // we must seek ahead until we have an unescaped IAC SE. If we don't have one, do nothing.
                                    bool escaped = false, match1 = false, match2 = false;
                                    while(b != end) {
                                        switch((uint8_t)*b) {
                                            case TelnetCode::IAC:
                                                if(match1) {
                                                    escaped = true;
                                                    match1 = false;
                                                } else {
                                                    match1 = true;
                                                }
                                                break;
                                            case TelnetCode::SB:
                                                if(match1) {
                                                    match2 = true;
                                                }
                                                // we have a winner!;
                                                b--;
                                                auto t = new TelnetMessage;
                                                t->msg_type = TelnetMsgType::Subnegotiation;
                                                t->codes[0] = option;
                                                std::copy(sub, b, t->data.begin());
                                                conn->inbox.consume(5 + t->data.size());
                                                conn->prot->receiveFromCodec(t);
                                        }
                                    }
                                } else {
                                    // Not enough data. wait for more.
                                    return;
                                }
                            } else {
                                // Yeah, it's a command...
                                auto t = new TelnetMessage;
                                t->msg_type = TelnetMsgType::Command;
                                t->codes[0] = (uint8_t)*b;
                                conn->prot->receiveFromCodec(t);
                                conn->inbox.consume(2);
                                continue;
                            }
                        }
                    }
                }
            } else {
                // Data begins on something that isn't an IAC. Scan ahead until we reach one...
                // Send all data up to an IAC, or everything if there is no IAC, as data.

                auto t = new TelnetMessage;
                t->msg_type = TelnetMsgType::AppData;
                auto check = std::find(begin, end, TelnetCode::IAC);
                std::copy(begin, check, t->data.begin());
                conn->inbox.consume(t->data.size());
                conn->prot->receiveFromCodec(t);
                continue;
            }
        }
    }

    void TelnetOption::registerHandshake() {
        if(supportLocal() && startDo()) {
            protocol->handshake_local.registerHandshake(opCode());
        }
        if(supportRemote() && startWill()) {
            protocol->handshake_remote.registerHandshake(opCode());
        }
    }
    bool TelnetOption::startWill() {return false;}
    bool TelnetOption::startDo() {return false;}
    bool TelnetOption::supportLocal() {return false;}
    bool TelnetOption::supportRemote() {return false;}

    void TelnetOption::negotiate(TelnetCode command) {
        switch(command) {
            case TelnetCode::DO:
                state.remote.negotiating = true;
                protocol->sendNegotiate(command, opCode());
                break;
            case TelnetCode::WILL:
                state.local.negotiating = true;
                protocol->sendNegotiate(command, opCode());
                break;
            default:
                break;
        }
    }

    void TelnetOption::onConnect() {
        if(startWill()) {
            negotiate(TelnetCode::WILL);
        }
        if(startDo()) {
            negotiate(TelnetCode::DO);
        }
    }

    void TelnetOption::receiveNegotiate(TelnetCode command) {
        switch(command) {
            case TelnetCode::WILL:
                if(supportRemote()) {
                    if(state.remote.negotiating) {
                        state.remote.negotiating = false;
                        if(!state.remote.enabled) {
                            state.remote.enabled = true;
                            enableRemote();
                            if(!state.remote.answered) {
                                state.remote.answered = true;
                                protocol->handshake_remote.processHandshake(opCode());
                            }
                        }
                    } else {
                        state.remote.enabled = true;
                        protocol->sendNegotiate(TelnetCode::DO, opCode());
                        enableRemote();
                        if(!state.remote.answered) {
                            state.remote.answered = true;
                            protocol->handshake_remote.processHandshake(opCode());
                        }
                    }
                } else {
                    protocol->sendNegotiate(TelnetCode::DONT, opCode());
                }
                break;
            case TelnetCode::DO:
                if(supportLocal()) {
                    if(state.local.negotiating) {
                        state.local.negotiating = false;
                        if(!state.local.enabled) {
                            state.local.enabled = true;
                            enableLocal();
                            if(!state.local.answered) {
                                state.local.answered = true;
                                protocol->handshake_local.processHandshake(opCode());
                            }
                        }
                    } else {
                        state.local.enabled = true;
                        protocol->sendNegotiate(TelnetCode::WILL, opCode());
                        enableLocal();
                        if(!state.local.answered) {
                            state.local.answered = true;
                            protocol->handshake_local.processHandshake(opCode());
                        }
                    }
                } else {
                    protocol->sendNegotiate(TelnetCode::WONT, opCode());
                }
                break;
            case TelnetCode::WONT:
                if(state.remote.enabled) disableRemote();
                if(state.remote.negotiating) {
                    state.remote.negotiating = false;
                    if(!state.remote.answered) {
                        state.remote.answered = true;
                        protocol->handshake_remote.processHandshake(opCode());
                    }
                }
                break;
            case TelnetCode::DONT:
                if(state.local.enabled) disableLocal();
                if(state.local.negotiating) {
                    state.local.negotiating = false;
                    if(!state.local.answered) {
                        state.local.answered = true;
                        protocol->handshake_local.processHandshake(opCode());
                    }
                }
                break;
            default:
                break;
        }
    }

    HandshakeHolder::HandshakeHolder(TelnetProtocol *p) {
        protocol = p;
    }

    void HandshakeHolder::registerHandshake(TelnetCode code) {
        handshakes.insert(code);
    }

    void HandshakeHolder::processHandshake(TelnetCode code) {
        handshakes.erase(code);
        if(empty()) {
            protocol->checkReady();
        }
    }

    bool HandshakeHolder::empty() {return handshakes.empty();}

    void TelnetProtocol::setConnection(net::Connection *c) {
        conn = c;
    }

    void TelnetProtocol::addHandler(TelnetOption *handler) {
        handlers[handler->opCode()] = handler;
        handler->protocol = this;
        handler->registerHandshake();
    }

    TelnetProtocol::TelnetProtocol(boost::asio::io_context& con) : Protocol(con), timer(con), handshake_local(this), handshake_remote(this),
    handshake_special(this) {
        addHandler(new MXPOption);
    }

    TelnetCode MXPOption::opCode() {return TelnetCode::MXP;}



    void TelnetProtocol::onConnect() {
        timer.expires_after(std::chrono::milliseconds(500));
        for(const auto & [k, v] : handlers) {
            v->onConnect();
        }
        timer.async_wait([&](std::error_code ec) {
            start();
        });
    }

    void TelnetProtocol::checkReady() {
        if(!handshaken) {
            if(handshake_local.empty() && handshake_remote.empty() && handshake_special.empty()) {
                start();
            }
        }
    }

    void TelnetProtocol::start() {
        if(!handshaken) {
            handshaken = true; // set this so further negotiations won't trigger another 'start'
            // send signal to the ConnectionManager here...
        }
    }

    void TelnetProtocol::onReceiveData(std::vector<uint8_t> &data, size_t length) {
        for(const auto & b : data) {
            overflow_counter++;
            if(overflow_counter > 16 * 1024) {
                // NOPE! none of this tomfoolery.
            }

        }
    }

    void TelnetProtocol::onClose() {

    }

    void TelnetProtocol::onLost() {

    }

    void TelnetProtocol::sendNegotiate(TelnetCode command, uint8_t option) {
        auto a = conn->outbox.prepare(3);
        uint8_t c[3] = {TelnetCode::IAC, command, option};
        memcpy(a.data(), &c, 3);
        conn->outbox.commit(3);
        conn->sendData();
    };

}