//
//  BotSession.h
//  BEAMBots
//
//  Created by Dmytro Lytovchenko on 2016-10-13.
//  Copyright © 2016 Epic Games, Inc. All rights reserved.
//

#pragma once

class ABEAMBotsToycar;

using BotSessionId = uint64_t;
class BotSession {
public:
    BotSessionId sid_;
    ABEAMBotsToycar *car_ = nullptr;
    double last_updated_;
    FString player_name_;
    
    BotSession(const BotSession &) = default;
    BotSession(BotSession &&) = default;
    
    BotSession(BotSessionId sid, const FString &name)
    : sid_(sid), player_name_(name) {
        touch();
    }
    
    void touch() {
        last_updated_ = FPlatformTime::Seconds();
    }
};

class BotResponse_SeeSelf {
public:
    bool is_good_ = false;
    FVector location_;
    FRotator rotation_;
};

class BotResponse_SeeObstacles {
public:
    bool is_good_ = false;
    // Contains distances to obstacle for casted rays.
    // Special value 1'000'000.0 means "too far" beyond the limit
    // of RAY_MAX_DIST=3'000cm.
    TArray<double> distances_;
    // Angle step. Default RAY_STEP=4°
    double ray_angle_step_;
};
