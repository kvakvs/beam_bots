#pragma once

#ifdef _WIN32
    #include "AllowWindowsPlatformTypes.h"
#endif
#include <zmq.hpp>
#undef PF_MAX
#ifdef _WIN32
    #include "HideWindowsPlatformTypes.h"
#endif

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
