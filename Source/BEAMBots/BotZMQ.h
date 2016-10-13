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
                            const zmq::message_t &m);
    void on_cmd_reset(ABEAMBotsGameMode *gmode,
                      const zmq::message_t &m);
    void on_cmd_ctl_motor(ABEAMBotsGameMode *gmode,
                          const zmq::message_t &m);
    
};

/// Writes big endian data types into a growing array buffer
class BotProtocol {
    FArrayWriter writer_;
public:
    BotProtocol(int estimated_size) : writer_() {
        writer_.Reserve(estimated_size);
    }
    void write_u8(uint8_t v) {
        writer_.Serialize(&v, sizeof(v));
    }
    void write_u64(uint64_t v) {
        if (FGenericPlatformProperties::IsLittleEndian()) { 
            v = BYTESWAP_ORDER64(v); 
        }
        writer_.Serialize(&v, sizeof(v));
    }
    const void *data() const { 
        return static_cast<const void *>(writer_.GetData());
    }
    int64_t size() {
        return writer_.TotalSize();
    }
};
