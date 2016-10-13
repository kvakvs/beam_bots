// Fill out your copyright notice in the Description page of Project Settings.

#include "BEAMBots.h"
#include "BEAMBotsGameMode.h"
#include "BotZMQ.h"
#include "BotSession.h"

static constexpr uint8_t PROTOCOL_VERSION = 0x00;

enum class BotCommand: uint8_t {
    ERROR         = 0x00,   // Responses with error string are marked with 0
    NEW_SESSION   = 0x01,
    RESET         = 0x02,
    CTL_MOTOR     = 0x03,
    SEE_SELF      = 0x04,
    SEE_OBSTACLES = 0x05,
};
    
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
        case BotCommand::NEW_SESSION:   return on_cmd_new_session(gmode, reader);
        case BotCommand::RESET:         return on_cmd_reset(gmode, reader);
        case BotCommand::CTL_MOTOR:     return on_cmd_ctl_motor(gmode, reader);
        case BotCommand::SEE_SELF:      return on_cmd_see_self(gmode, reader);
        case BotCommand::SEE_OBSTACLES: return on_cmd_see_obstacles(gmode, reader);
    }
}

void BotZMQ::on_cmd_new_session(ABEAMBotsGameMode *gmode,
                                BotProtocolReader &reader) {
    uint8_t ver = reader.read_u8();
    if (ver != PROTOCOL_VERSION) {
        return send_error(0ULL, // no session id = 0
                          FString::Printf(TEXT("Protocol version mismatch, expected %d"),
                                          PROTOCOL_VERSION));
    }
    
    FString player_name = reader.read_string();
    BotSessionId sid = gmode->bot_new_session(player_name);
    UE_LOG(LogTemp, Log, TEXT("Login for %s"), *player_name);
    
    // Compose and send reply
    BotProtocol message;
    message.write_u8((uint8_t)BotCommand::NEW_SESSION);
    message.write_u64(sid);
    
    socket_.send(message.data(), message.size());
}
    
void BotZMQ::on_cmd_reset(ABEAMBotsGameMode *gmode,
                          BotProtocolReader &reader) {
    auto sid = reader.read_u64();
    if (not gmode->bot_reset(sid)) {
        return send_error(sid, "RESET command failed");
    }

    // Compose and send reply
    BotProtocol message;
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
    
    if (not gmode->bot_control_motors(sid, fl, fr, bl, br)) {
        return send_error(sid, "CTL_MOTOR command failed");
    }

    // Compose and send reply
    BotProtocol message;
    message.write_u8((uint8_t)BotCommand::CTL_MOTOR);
    message.write_u64(sid);
    
    socket_.send(message.data(), message.size());
}

void BotZMQ::on_cmd_see_self(ABEAMBotsGameMode *gmode,
                             BotProtocolReader &reader) {
    auto sid = reader.read_u64();
    BotResponse_SeeSelf info = gmode->bot_see_self(sid);
    if (not info.is_good_) {
        return send_error(sid, "SEE_SELF command failed");
    }

    // Compose and send reply
    BotProtocol message;
    message.write_u8((uint8_t)BotCommand::SEE_SELF);
    message.write_u64(sid);
    message.write_f32(info.location_.X);
    message.write_f32(info.location_.Y);
    message.write_f32(info.location_.Z);
    message.write_f32(info.rotation_.Roll);
    message.write_f32(info.rotation_.Pitch);
    message.write_f32(info.rotation_.Yaw);
    
    socket_.send(message.data(), message.size());
}

void BotZMQ::on_cmd_see_obstacles(ABEAMBotsGameMode *gmode,
                                  BotProtocolReader &reader) {
    
}

void BotZMQ::send_error(BotSessionId sid, FString e) {
    BotProtocol message;
    message.write_u8((uint8_t)BotCommand::ERROR);
    message.write_string(e);
    socket_.send(message.data(), message.size());
}

