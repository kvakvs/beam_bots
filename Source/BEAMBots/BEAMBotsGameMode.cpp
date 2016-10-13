// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BEAMBots.h"
#include "BEAMBotsGameMode.h"
#include "BEAMBotsHud.h"
#include "BEAMBotsToyCar.h"
#include "BEAMBotsObserverPawn.h"

#include "Runtime/Engine/Classes/Engine/World.h"

static const double EXPIRE_SESSION = 60.0;

ABEAMBotsGameMode::ABEAMBotsGameMode(): AGameMode()
{
    //DefaultPawnClass = ABEAMBotsPawn::StaticClass();
    //DefaultPawnClass = ABEAMBotsToycar::StaticClass();
    DefaultPawnClass = ABEAMBotsObserverPawn::StaticClass();
    HUDClass = ABEAMBotsHud::StaticClass();
}

void ABEAMBotsGameMode::StartPlay()
{
    auto mode = GetWorld()->WorldType;
    switch (mode) {
    case EWorldType::Game:
    case EWorldType::PIE:
        zmq_.Reset(new BotZMQ(15000));
    }
    
    AGameMode::StartPlay();
}

void ABEAMBotsGameMode::Tick(float dt) {
    zmq_->tick(this);

    expire_sessions_timer_ += dt;
    if (expire_sessions_timer_ >= 5.0) {
        expire_sessions_timer_ = 0.0;
        double expire = FPlatformTime::Seconds() - EXPIRE_SESSION;
        TArray<BotSessionId> expired_ids;

        for (auto &ses: sessions_) {
            if (ses.Value.last_updated_ < expire) {
                expired_ids.Add(ses.Key);
            }
        }
    }
}

BotSessionId ABEAMBotsGameMode::bot_new_session(const FString & player_name)
{
    BotSession s;
    
    do {
        // Assign a random number sid until it's unique
        s.sid_ = ((uint64_t)FMath::Rand() << 32) ^ FMath::Rand();
    } while (sessions_.Find(s.sid_) != nullptr);

    sessions_.Add(s.sid_, s);
    return s.sid_;
}

bool ABEAMBotsGameMode::bot_reset(BotSessionId sid)
{
    BotSession *s = sessions_.Find(sid);
    if (s == nullptr) {
        return false;
    }
    
    auto world = GetWorld();
    check(world);
    s->car_ = world->SpawnActor<ABEAMBotsToycar>(ABEAMBotsToycar::StaticClass());
    s->touch();
    return true;
}
