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

