// Fill out your copyright notice in the Description page of Project Settings.

#include "BEAMBots.h"
#include "BEAMBotsGameMode.h"
#include "BotZMQ.h"

enum class ProtocolCmd: unsigned char {
    NEW_SESSION = 0x01,
    RESET = 0x02,
    CTL_MOTOR = 0x03,
};
    
static FString read_string(const zmq::message_t &m, int offset);
    
BotZMQ::BotZMQ(uint32 port) : context_(1), socket_(context_, ZMQ_REP)
{
    //while (port)
    FString listen_str = FString::Printf(TEXT("tcp://*:%u"), port);
    socket_.bind(TCHAR_TO_ANSI(*listen_str));
}

void BotZMQ::tick(ABEAMBotsGameMode *gmode) {
    zmq::pollitem_t poll_items[] = {
        {socket_, 0, ZMQ_POLLIN, 0}
    };
    zmq::poll(poll_items, 1, 0);
    if (poll_items[0].revents & ZMQ_POLLIN) {
        zmq::message_t message;
        socket_.recv(& message);
        parse_message(gmode, message);
    }
}

void BotZMQ::parse_message(ABEAMBotsGameMode *gmode, const zmq::message_t &m) {
    if (m.size() == 0) {
        return;
    }
    const char *data = static_cast<const char *>(m.data());
    switch (ProtocolCmd(data[0])) {
        case ProtocolCmd::NEW_SESSION:  return on_cmd_new_session(gmode, m);
        case ProtocolCmd::RESET:        return on_cmd_reset(gmode, m);
        case ProtocolCmd::CTL_MOTOR:    return on_cmd_ctl_motor(gmode, m);
    }
}

void BotZMQ::on_cmd_new_session(ABEAMBotsGameMode *gmode,
                                const zmq::message_t &m) {
    const char *data = static_cast<const char *>(m.data());
    FString player_name = read_string(m, 1);
    UE_LOG(LogTemp, Log, TEXT("Login for %s"), *player_name);
    
    // Compose and send reply
    uint64_t sid = 123;
    char msg[sizeof(sid)+1];
    
    msg[0] = (char)ProtocolCmd::NEW_SESSION;
    memcpy(msg+1, &sid, sizeof(sid));
    
    socket_.send(&msg, sizeof(sid)+1);
}
    
void BotZMQ::on_cmd_reset(ABEAMBotsGameMode *gmode,
                          const zmq::message_t &m) {
}
    
void BotZMQ::on_cmd_ctl_motor(ABEAMBotsGameMode *gmode,
                              const zmq::message_t &m) {
}

FString read_string(const zmq::message_t &m, int offset) {
    const char *data = static_cast<const char *>(m.data());
    unsigned char length = data[offset];
    
//    std::wstring_convert<std::codecvt_utf8_utf16<TCHAR>> converter;
    auto size = std::min<int>(length, m.size() - offset);
    const char *in = data + offset + 1;
    TUniquePtr<TCHAR> out(new TCHAR[1+size]);
//    std::string ascii_str(data + offset + 1, size);
//    return FString(size, converter.from_bytes(ascii_str).c_str());
    TStringConvert<char, TCHAR>::Convert(out.Get(), size, in, size);
    return FString(size, out.Get());
}
