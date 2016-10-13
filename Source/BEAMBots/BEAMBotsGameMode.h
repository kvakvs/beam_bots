// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "BotZMQ.h"
#include <memory>

#include "GameFramework/GameMode.h"

class ABEAMBotsToycar;

using BotSessionId = uint64_t;
class BotSession {
public:
    BotSessionId sid_;
    ABEAMBotsToycar *car_ = nullptr;
    double last_updated_;

    BotSession(const BotSession &) = default;
    BotSession(BotSession &&) = default;
    BotSession() {
        touch();
    }
    void touch() {
        last_updated_ = FPlatformTime::Seconds();
    }
};

#include "BEAMBotsGameMode.generated.h"

UCLASS(minimalapi)
class ABEAMBotsGameMode : public AGameMode
{
    GENERATED_BODY()
    //std::unique_ptr<BotZMQ> zmq_;
    TUniquePtr<BotZMQ> zmq_;

    // Maps random session ids to players
    TMap<BotSessionId, BotSession> sessions_;
    double expire_sessions_timer_ = 0.0;

public:
    ABEAMBotsGameMode();
    virtual void StartPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    
    BotSessionId bot_new_session(const FString &player_name);
    bool bot_reset(BotSessionId sid);
private:
};
