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
    
    auto start = this->ChoosePlayerStart(world->GetFirstPlayerController());
    
    s->car_ = world->SpawnActor<ABEAMBotsToycar>(ABEAMBotsToycar::StaticClass());
    s->car_->TeleportTo(start->GetActorLocation(),
                        start->GetActorRotation());
    
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

BotResponse_SeeObstacles ABEAMBotsGameMode::bot_see_obstacles(BotSessionId sid) {
    BotResponse_SeeObstacles result;
    BotSession *s = sessions_.Find(sid);
    
    if (not s or not s->car_) {
        return result; // has is_good_ == false
    }
    
    constexpr int RAY_COUNT = 16;
    constexpr float RAY_STEP = 4.0f;
    constexpr float RAY_MAX_DIST = 3000.f;
    constexpr float RAY_TOO_FAR = 1000000.0f;
    constexpr float DEBUG_RAY_LIFETIME = 0.25f;
    constexpr float SENSOR_HEIGHT = -100.0f;
    constexpr float CAR_LENGTH = 400.f;
    
    result.is_good_ = true;
    result.ray_angle_step_ = RAY_STEP;
    result.distances_.Reserve(RAY_COUNT);

    auto &car = *(s->car_);
    auto rotation = car.GetActorRotation();

    FVector sensor_pos(CAR_LENGTH, 0.f, SENSOR_HEIGHT);
    auto ray_start = car.GetActorLocation() + rotation.RotateVector(sensor_pos);
    
    auto world = GetWorld();

    double angle = rotation.Yaw - (RAY_COUNT * 0.5f) * RAY_STEP;
    FVector forward(RAY_MAX_DIST, 0.f, 0.f);
    
    for (auto i = 0; i < RAY_COUNT; ++i) {
        rotation.Yaw = angle;
        FVector ray_end = ray_start + rotation.RotateVector(forward);
        
        DrawDebugLine(world, ray_start, ray_end, FColor::Red, false,
                      DEBUG_RAY_LIFETIME);
        
        FHitResult hit1, hit2;
        bool found1 = world->LineTraceSingleByChannel(
            hit1, ray_start, ray_end, ECollisionChannel::ECC_WorldStatic);
        bool found2 = world->LineTraceSingleByChannel(
            hit2, ray_start, ray_end, ECollisionChannel::ECC_PhysicsBody);
        
        float dist = RAY_TOO_FAR;
        if (found1) {
            dist = hit1.Distance;
        }
        if (found2) {
            dist = std::min<float>(dist, hit2.Distance);
        }
        result.distances_.Add(dist);
        angle += RAY_STEP;
    }
    // WorldStatic, PhysicsBody
    return result;
}

