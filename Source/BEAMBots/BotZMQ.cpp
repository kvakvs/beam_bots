// Fill out your copyright notice in the Description page of Project Settings.

#include "BEAMBots.h"
#include "BEAMBotsGameMode.h"
#include "BotZMQ.h"

enum class BotCommand: uint8_t {
    NEW_SESSION = 0x01,
    RESET       = 0x02,
    CTL_MOTOR   = 0x03,
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
    BotProtocolReader reader(static_cast<const uint8_t *>(m.data()), m.size());
    switch (BotCommand(reader.read_u8())) {
        case BotCommand::NEW_SESSION:  return on_cmd_new_session(gmode, reader);
        case BotCommand::RESET:        return on_cmd_reset(gmode, reader);
        case BotCommand::CTL_MOTOR:    return on_cmd_ctl_motor(gmode, reader);
    }
}

void BotZMQ::on_cmd_new_session(ABEAMBotsGameMode *gmode,
                                BotProtocolReader &reader) {
    FString player_name = reader.read_string();
    BotSessionId sid = gmode->bot_new_session(player_name);
    UE_LOG(LogTemp, Log, TEXT("Login for %s"), *player_name);
    
    // Compose and send reply
    BotProtocol message(9);
    message.write_u8((uint8_t)BotCommand::NEW_SESSION);
    message.write_u64(sid);
    
    socket_.send(message.data(), message.size());
}
    
void BotZMQ::on_cmd_reset(ABEAMBotsGameMode *gmode,
                          BotProtocolReader &reader) {
    auto sid = reader.read_u64();
    gmode->bot_reset(sid);

    // Compose and send reply
    BotProtocol message(1);
    message.write_u8((uint8_t)BotCommand::RESET);
    message.write_u64(sid);
    
    socket_.send(message.data(), message.size());
}
    
void BotZMQ::on_cmd_ctl_motor(ABEAMBotsGameMode *gmode,
                              BotProtocolReader &reader) {
    auto sid = reader.read_u64();
    double fl = reader.read_f32();
    double fr = reader.read_f32();
    double bl = reader.read_f32();
    double br = reader.read_f32();
    gmode->bot_control_motors(sid, fl, fr, bl, br);

    // Compose and send reply
    BotProtocol message(9);
    message.write_u8((uint8_t)BotCommand::CTL_MOTOR);
    message.write_u64(sid);
    
    socket_.send(message.data(), message.size());
}

FString BotProtocolReader::read_string() {
    uint64_t length = read_u8();
    length = std::min<uint64_t>(length, size() - offset_);
    
    const char *in = (const char *)(data_.GetData() + offset_);
    TUniquePtr<TCHAR> out(new TCHAR[1 + length]);

    TStringConvert<char, TCHAR>::Convert(out.Get(), length, in, length);
    return FString(length, out.Get());
}
