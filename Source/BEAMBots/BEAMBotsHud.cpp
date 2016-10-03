// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BEAMBots.h"
#include "BEAMBotsHud.h"
#include "BEAMBotsPawn.h"
#include "GameFramework/WheeledVehicle.h"
#include "RenderResource.h"
#include "Shader.h"
#include "Engine/Canvas.h"
#include "Vehicles/WheeledVehicleMovementComponent.h"
#include "Engine/Font.h"
#include "CanvasItem.h"
#include "Engine.h"

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IHeadMountedDisplay.h"
#endif // HMD_MODULE_INCLUDED 

#define LOCTEXT_NAMESPACE "VehicleHUD"

ABEAMBotsHud::ABEAMBotsHud()
{
    static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
    HUDFont = Font.Object;
}

void ABEAMBotsHud::DrawHUD()
{
    Super::DrawHUD();

    // Calculate ratio from 720p
    const float HUDXRatio = Canvas->SizeX / 1280.f;
    const float HUDYRatio = Canvas->SizeY / 720.f;

    bool want_hud_ = true;
#if HMD_MODULE_INCLUDED
    if (GEngine->HMDDevice.IsValid() == true) {
        want_hud_ = GEngine->HMDDevice->IsStereoEnabled();
    }
#endif // HMD_MODULE_INCLUDED
    // We dont want the onscreen hud when using a HMD device	
    if (want_hud_ == true) {
        // Get our vehicle so we can check if we are in car. If we are we don't want onscreen HUD
        ABEAMBotsPawn* veh_ = Cast<ABEAMBotsPawn>(GetOwningPawn());
        if ((veh_ != nullptr) && (veh_->incar_camera_active_ == false)) {
            FVector2D ScaleVec(HUDYRatio * 1.4f, HUDYRatio * 1.4f);

            // Speed
            FCanvasTextItem speed_txt_(FVector2D(HUDXRatio * 805.f, HUDYRatio * 455),
                                       veh_->speed_disp_str_, HUDFont, FLinearColor::White);
            speed_txt_.Scale = ScaleVec;
            Canvas->DrawItem(speed_txt_);

            // Gear
            FCanvasTextItem gear_txt_(FVector2D(HUDXRatio * 805.f, HUDYRatio * 500.f), 
                                      veh_->gear_disp_str_, HUDFont, 
                                      veh_->is_reverse_gear_ == false ? veh_->gear_disp_color_ 
                                                                      : veh_->gear_disp_reverse_color_);
            gear_txt_.Scale = ScaleVec;
            Canvas->DrawItem(gear_txt_);
        }
    }
}


#undef LOCTEXT_NAMESPACE
