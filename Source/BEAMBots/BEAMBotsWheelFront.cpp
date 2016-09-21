// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BEAMBots.h"
#include "BEAMBotsWheelFront.h"

UBEAMBotsWheelFront::UBEAMBotsWheelFront()
{
    ShapeRadius = 35.f;
    ShapeWidth = 10.0f;
    bAffectedByHandbrake = false;
    SteerAngle = 50.f;
}
