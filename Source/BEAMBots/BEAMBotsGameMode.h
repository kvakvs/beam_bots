// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "BotZMQ.h"
#include <memory>

#include "GameFramework/GameMode.h"
#include "BEAMBotsGameMode.generated.h"

UCLASS(minimalapi)
class ABEAMBotsGameMode : public AGameMode
{
    GENERATED_BODY()
    std::unique_ptr<BotZMQ> zmq_;

public:
    ABEAMBotsGameMode();
    virtual void StartPlay() override;

private:
    void zmq_listen(uint32 port);
};
