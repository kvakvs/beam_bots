// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BEAMBots.h"
#include "BEAMBotsPawn.h"
#include "BEAMBotsWheelFront.h"
#include "BEAMBotsWheelRear.h"
#include "BEAMBotsHud.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Vehicles/WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine.h"

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IHeadMountedDisplay.h"
#endif // HMD_MODULE_INCLUDED

const FName ABEAMBotsPawn::LookUpBinding("LookUp");
const FName ABEAMBotsPawn::LookRightBinding("LookRight");

#define LOCTEXT_NAMESPACE "VehiclePawn"

ABEAMBotsPawn::ABEAMBotsPawn()
{
    // Car mesh
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(
        TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
    GetMesh()->SetSkeletalMesh(CarMesh.Object);

    static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(
        TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
    GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

    // Simulation
    UWheeledVehicleMovementComponent4W* Vehicle4W 
        = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

    check(Vehicle4W->WheelSetups.Num() == 4);

    Vehicle4W->WheelSetups[0].WheelClass = UBEAMBotsWheelFront::StaticClass();
    Vehicle4W->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
    Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -12.f, 0.f);

    Vehicle4W->WheelSetups[1].WheelClass = UBEAMBotsWheelFront::StaticClass();
    Vehicle4W->WheelSetups[1].BoneName = FName("Wheel_Front_Right");
    Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 12.f, 0.f);

    Vehicle4W->WheelSetups[2].WheelClass = UBEAMBotsWheelRear::StaticClass();
    Vehicle4W->WheelSetups[2].BoneName = FName("Wheel_Rear_Left");
    Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -12.f, 0.f);

    Vehicle4W->WheelSetups[3].WheelClass = UBEAMBotsWheelRear::StaticClass();
    Vehicle4W->WheelSetups[3].BoneName = FName("Wheel_Rear_Right");
    Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 12.f, 0.f);

    // Create a spring arm component
    //spring_arm_ = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
    //spring_arm_->TargetOffset = FVector(0.f, 0.f, 200.f);
    //spring_arm_->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
    //spring_arm_->SetupAttachment(RootComponent);
    //spring_arm_->TargetArmLength = 600.0f;
    //spring_arm_->bEnableCameraRotationLag = true;
    //spring_arm_->CameraRotationLagSpeed = 7.f;
    //spring_arm_->bInheritPitch = false;
    //spring_arm_->bInheritRoll = false;

    // Create camera component 
    //cam_ = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
    //cam_->SetupAttachment(spring_arm_, USpringArmComponent::SocketName);
    //cam_->bUsePawnControlRotation = false;
    //cam_->FieldOfView = 90.f;

    // Create In-Car camera component 
    //internal_camera_org_ = FVector(0.0f, -40.0f, 120.0f);

    //internal_cam_base_ = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
    //internal_cam_base_->SetRelativeLocation(internal_camera_org_);
    //internal_cam_base_->SetupAttachment(GetMesh());

    //internal_cam_ = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
    //internal_cam_->bUsePawnControlRotation = false;
    //internal_cam_->FieldOfView = 90.f;
    //internal_cam_->SetupAttachment(internal_cam_base_);

    //Setup TextRenderMaterial
    static ConstructorHelpers::FObjectFinder<UMaterial> TextMaterial(
        TEXT("Material'/Engine/EngineMaterials/AntiAliasedTextMaterialTranslucent.AntiAliasedTextMaterialTranslucent'"));

    UMaterialInterface* Material = TextMaterial.Object;

    // Create text render component for in car speed display
    //incar_speed_ = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
    //incar_speed_->SetTextMaterial(Material);
    //incar_speed_->SetRelativeLocation(FVector(70.0f, -75.0f, 99.0f));
    //incar_speed_->SetRelativeRotation(FRotator(18.0f, 180.0f, 0.0f));
    //incar_speed_->SetupAttachment(GetMesh());
    //incar_speed_->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));

    // Create text render component for in car gear display
    //incar_gear_ = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
    //incar_gear_->SetTextMaterial(Material);
    //incar_gear_->SetRelativeLocation(FVector(66.0f, -9.0f, 95.0f));
    //incar_gear_->SetRelativeRotation(FRotator(25.0f, 180.0f, 0.0f));
    //incar_gear_->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));
    //incar_gear_->SetupAttachment(GetMesh());

    // Colors for the incar gear display. One for normal one for reverse
    gear_disp_reverse_color_ = FColor(255, 0, 0, 255);
    gear_disp_color_ = FColor(255, 255, 255, 255);

    // Colors for the in-car gear display. One for normal one for reverse
    gear_disp_reverse_color_ = FColor(255, 0, 0, 255);
    gear_disp_color_ = FColor(255, 255, 255, 255);

    is_reverse_gear_ = false;
}

void ABEAMBotsPawn::SetupPlayerInputComponent(class UInputComponent* pic)
{
    Super::SetupPlayerInputComponent(pic);

    // set up gameplay key bindings
    check(pic);

    pic->BindAxis("MoveForward", this, &ABEAMBotsPawn::MoveForward);
    pic->BindAxis("MoveRight", this, &ABEAMBotsPawn::MoveRight);
    pic->BindAxis("LookUp");
    pic->BindAxis("LookRight");

    pic->BindAction("Handbrake", IE_Pressed, this, &ABEAMBotsPawn::OnHandbrakePressed);
    pic->BindAction("Handbrake", IE_Released, this, &ABEAMBotsPawn::OnHandbrakeReleased);
    //pic->BindAction("SwitchCamera", IE_Pressed, this, &ABEAMBotsPawn::OnToggleCamera);

    pic->BindAction("ResetVR", IE_Pressed, this, &ABEAMBotsPawn::OnResetVR);
}

void ABEAMBotsPawn::MoveForward(float Val)
{
    GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void ABEAMBotsPawn::MoveRight(float Val)
{
    GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void ABEAMBotsPawn::OnHandbrakePressed()
{
    GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void ABEAMBotsPawn::OnHandbrakeReleased()
{
    GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void ABEAMBotsPawn::OnToggleCamera()
{
    EnableIncarView(!incar_camera_active_);
}

void ABEAMBotsPawn::EnableIncarView(const bool bState, const bool bForce)
{
    //if ((bState != incar_camera_active_) || (bForce == true))
    //{
    //    incar_camera_active_ = bState;

    //    if (bState == true)
    //    {
    //        OnResetVR();
    //        cam_->Deactivate();
    //        internal_cam_->Activate();
    //    }
    //    else
    //    {
    //        internal_cam_->Deactivate();
    //        cam_->Activate();
    //    }

    //    incar_speed_->SetVisibility(incar_camera_active_);
    //    incar_gear_->SetVisibility(incar_camera_active_);
    //}
}


void ABEAMBotsPawn::Tick(float Delta)
{
    Super::Tick(Delta);

    // Setup the flag to say we are in reverse gear
    is_reverse_gear_ = GetVehicleMovement()->GetCurrentGear() < 0;

    // Update the strings used in the hud (incar and onscreen)
    UpdateHUDStrings();

    // Set the string in the incar hud
    SetupInCarHUD();

    bool bHMDActive = false;
#if HMD_MODULE_INCLUDED
    if ((GEngine->HMDDevice.IsValid() == true) && ((GEngine->HMDDevice->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true)))
    {
        bHMDActive = true;
    }
#endif // HMD_MODULE_INCLUDED
    if (bHMDActive == false)
    {
        //if ((InputComponent) && (incar_camera_active_ == true))
        //{
        //    FRotator HeadRotation = internal_cam_->RelativeRotation;
        //    HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
        //    HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
        //    internal_cam_->RelativeRotation = HeadRotation;
        //}
    }
}

void ABEAMBotsPawn::BeginPlay()
{
    Super::BeginPlay();

    bool bEnableInCar = false;
#if HMD_MODULE_INCLUDED
    bEnableInCar = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
#endif // HMD_MODULE_INCLUDED
    //EnableIncarView(bEnableInCar, true);

    // Setup camera
    for (TActorIterator<ACameraActor> iter(GetWorld()); iter; ++iter) {
        if (iter->GetName() == "TopCamera") {
            auto pc = GetWorld()->GetFirstPlayerController();
            //auto pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            pc->SetViewTarget(*iter, FViewTargetTransitionParams());
            break;
        }
    }
}

void ABEAMBotsPawn::OnResetVR()
{
#if HMD_MODULE_INCLUDED
    if (GEngine->HMDDevice.IsValid())
    {
        GEngine->HMDDevice->ResetOrientationAndPosition();
        internal_cam_->SetRelativeLocation(internal_camera_org_);
        GetController()->SetControlRotation(FRotator());
    }
#endif // HMD_MODULE_INCLUDED
}

void ABEAMBotsPawn::UpdateHUDStrings()
{
    float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
    int32 KPH_int = FMath::FloorToInt(KPH);

    // Using FText because this is display text that should be localizable
    speed_disp_str_ = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));

    if (is_reverse_gear_ == true)
    {
        gear_disp_str_ = FText(LOCTEXT("ReverseGear", "R"));
    }
    else
    {
        int32 Gear = GetVehicleMovement()->GetCurrentGear();
        gear_disp_str_ = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
    }
}

void ABEAMBotsPawn::SetupInCarHUD()
{
    //APlayerController* PlayerController = Cast<APlayerController>(GetController());
    //if ((PlayerController != nullptr) && (incar_speed_ != nullptr) && (incar_gear_ != nullptr))
    //{
    //    // Setup the text render component strings
    //    incar_speed_->SetText(speed_disp_str_);
    //    incar_gear_->SetText(gear_disp_str_);

    //    if (is_reverse_gear_ == false)
    //    {
    //        incar_gear_->SetTextRenderColor(gear_disp_color_);
    //    }
    //    else
    //    {
    //        incar_gear_->SetTextRenderColor(gear_disp_reverse_color_);
    //    }
    //}
}

#undef LOCTEXT_NAMESPACE
