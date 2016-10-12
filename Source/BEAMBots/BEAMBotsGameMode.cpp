// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BEAMBots.h"
#include "BEAMBotsGameMode.h"
#include "BEAMBotsHud.h"
#include "BEAMBotsToyCar.h"

#include "Runtime/Engine/Classes/Engine/World.h"

ABEAMBotsGameMode::ABEAMBotsGameMode(): AGameMode()
{
    //DefaultPawnClass = ABEAMBotsPawn::StaticClass();
    DefaultPawnClass = ABEAMBotsToycar::StaticClass();
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

void ABEAMBotsGameMode::Tick(float DeltaSeconds) {
    zmq_->tick(this);
}