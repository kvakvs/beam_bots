// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BEAMBots.h"
#include "BEAMBotsGameMode.h"
//#include "BEAMBotsPawn.h"
#include "BrickPawn.h"
#include "BEAMBotsHud.h"

#include "Runtime/Engine/Classes/Engine/World.h"

ABEAMBotsGameMode::ABEAMBotsGameMode(): AGameMode()
{
    //DefaultPawnClass = ABEAMBotsPawn::StaticClass();
    DefaultPawnClass = ABrickPawn::StaticClass();
    HUDClass = ABEAMBotsHud::StaticClass();
}

void ABEAMBotsGameMode::zmq_listen(uint32 port)
{
    //zmq_ = std::make_unique<BotZMQ>(port);
    zmq_.Reset(new BotZMQ(port));
}

void ABEAMBotsGameMode::StartPlay()
{
    auto mode = GetWorld()->WorldType;
    switch (mode) {
    case EWorldType::Game:
    case EWorldType::PIE:
        zmq_listen(15000); 
    }
    AGameMode::StartPlay();
}
