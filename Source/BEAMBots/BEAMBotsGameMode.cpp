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
        
        for (BotSessionId id: expired_ids) {
            BotSession *s = sessions_.Find(id);
            if (s->car_) {
                s->car_->Destroy();
                s->car_ = nullptr;
            }
        }
    }
}

BotSessionId ABEAMBotsGameMode::bot_new_session(const FString & player_name)
{
    BotSessionId sid = 0;
    do {
        // Assign a random number sid until it's unique
        sid = ((uint64_t)FMath::Rand() << 32) ^ FMath::Rand();
    } while (sessions_.Find(sid) != nullptr);
    
    BotSession s(sid, player_name);
    
    sessions_.Add(s.sid_, s);
    return s.sid_;
}

bool ABEAMBotsGameMode::bot_reset(BotSessionId sid)
{
    BotSession *s = sessions_.Find(sid);
    if (not s) {
        return false;
    }
    
    if (s->car_) {
        // Despawn old car first
        s->car_->Destroy();
        s->car_ = nullptr;
    }
    
    auto world = GetWorld();
    check(world);
    s->car_ = world->SpawnActor<ABEAMBotsToycar>(ABEAMBotsToycar::StaticClass());
    s->car_->player_name_ = s->player_name_;
    s->touch();
    return true;
}

bool ABEAMBotsGameMode::bot_control_motors(BotSessionId sid,
                                           double fl, double fr,
                                           double bl, double br)
{
    BotSession *s = sessions_.Find(sid);
    if (not s) {
        return false;
    }
    
    s->car_->power_motor(s->car_->motor_index_FL, fl);
    s->car_->power_motor(s->car_->motor_index_FR, fr);
    s->car_->power_motor(s->car_->motor_index_BL, bl);
    s->car_->power_motor(s->car_->motor_index_BR, br);
    s->touch();
    return true;
}

BotResponse_SeeSelf ABEAMBotsGameMode::bot_see_self(BotSessionId sid) {
    BotResponse_SeeSelf result;
    BotSession *s = sessions_.Find(sid);

    if (not s or not s->car_) {
        return result; // has is_good_ == false
    }

    result.is_good_ = true;
    result.location_ = s->car_->GetActorLocation();
    result.rotation_ = s->car_->GetActorRotation();
    return result;
}
