#pragma once

#ifdef _WIN32
    #include "AllowWindowsPlatformTypes.h"
#endif
#include "zmq.hpp"
#undef PF_MAX
#ifdef _WIN32
    #include "HideWindowsPlatformTypes.h"
#endif

#include "UnrealString.h"
#include "BotSession.h"
#include "BotProtocol.h"

class ABEAMBotsGameMode;


/**
 * Wrapper for ZeroMQ Library to avoid mixing Unreal vs. Windows includes
 */
class BEAMBOTS_API BotZMQ
{
    zmq::context_t context_;
    zmq::socket_t socket_;

public:
    BotZMQ(uint32 port);
    virtual ~BotZMQ() {}
    void tick(ABEAMBotsGameMode *gmode);
    
private:
    void parse_message(ABEAMBotsGameMode *gmode,
                       const zmq::message_t &m);
    void on_cmd_new_session(ABEAMBotsGameMode *gmode,
                            BotProtocolReader &reader);
    void on_cmd_reset(ABEAMBotsGameMode *gmode,
                      BotProtocolReader &reader);
    void on_cmd_ctl_motor(ABEAMBotsGameMode *gmode,
                          BotProtocolReader &reader);
    void on_cmd_see_self(ABEAMBotsGameMode *gmode,
                         BotProtocolReader &reader);
    void on_cmd_see_obstacles(ABEAMBotsGameMode *gmode,
                              BotProtocolReader &reader);
    
    void send_error(BotSessionId sid, FString e);
};
