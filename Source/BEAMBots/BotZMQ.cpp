// Fill out your copyright notice in the Description page of Project Settings.

#include "BEAMBots.h"
#include "BotZMQ.h"

BotZMQ::BotZMQ(uint32 port) : context_(1), socket_(context_, ZMQ_REP)
{
    //while (port)
    FString listen_str = FString::Printf(TEXT("tcp://*:%u"), port);
    socket_.bind(TCHAR_TO_ANSI(*listen_str));
}
