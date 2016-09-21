#pragma once

#include "AllowWindowsPlatformTypes.h"
#include <zmq.hpp>
#undef PF_MAX
#include "HideWindowsPlatformTypes.h"

#include "UnrealString.h"

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
};
