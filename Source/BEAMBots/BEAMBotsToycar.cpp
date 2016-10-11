// Made by: Dmytro 'kvakvs' Lytovchenko

#include "BEAMBots.h"
#include "BEAMBotsToycar.h"
#include "Engine.h"

#include "GenericPlatformMath.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"

// #define OBSERVER_CAMERA_NAME "TopCamera"
#define OBSERVER_CAMERA_NAME "TopCloseupCamera"

// Sets default values
ABEAMBotsToycar::ABEAMBotsToycar()
{
    construct_geometry();
}

void ABEAMBotsToycar::construct_geometry() {
    // Car mesh
    mesh_ = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> mesh_helper(
        TEXT("SkeletalMesh'/Game/ToyCars/ToyCar1.ToyCar1'"));
    mesh_->SetSkeletalMesh(mesh_helper.Object);
   
    mesh_->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    mesh_->SetSimulatePhysics(true);
    
    // Color+material
    //static ConstructorHelpers::FObjectFinder<UMaterial> material_helper(
    //    TEXT("Material'/Game/Materials/CarPaint/M_CarPaint'"));
    //mesh_->SetMaterial(0, material_helper.Object);

    SetRootComponent(mesh_);
    
    // Camera
    // Create a spring arm component
    spring_arm_ = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
    spring_arm_->TargetOffset = FVector(0.f, 0.f, 200.f);
    spring_arm_->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
    spring_arm_->SetupAttachment(RootComponent);
    spring_arm_->TargetArmLength = 600.0f;
    spring_arm_->bEnableCameraRotationLag = true;
    spring_arm_->CameraRotationLagSpeed = 7.f;
    spring_arm_->bInheritPitch = false;
    spring_arm_->bInheritRoll = false;
    
    // Create camera component
    cam_ = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
    cam_->SetupAttachment(spring_arm_, USpringArmComponent::SocketName);
    cam_->bUsePawnControlRotation = false;
    cam_->FieldOfView = 90.f;

    //auto l = CreateDefaultSubobject<UPointLightComponent>(TEXT("FullBeam"));
    //l->SetRelativeLocation(FVector(60.f, -10.f, 120.f));
    //l->SetRelativeRotation(FRotator(-180.f, -10.f, -90.f));
    //l->SetIntensity(25000.f);
}

// Called when the game starts or when spawned
void ABEAMBotsToycar::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ABEAMBotsToycar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (wake_flag_) {
        wake_flag_ = false;
        mesh_->WakeAllRigidBodies();
    }
    for (auto i = 0; i < mesh_->Constraints.Num(); ++i) {
        mesh_->Constraints[i]->SetAngularVelocityTarget(drive_power_[i]);
    }

    FVector dir = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
    dir.Normalize();

    FVector fl = drive_power_[DrivePower_FL];
    fl.Normalize();
    fl *= 2000.f;
    fl *= dir;
    mesh_->AddImpulseAtLocation(
        fl, mesh_->Constraints[DrivePower_FL]->GetConstraintLocation());

    FVector fr = drive_power_[DrivePower_FR];
    fr.Normalize();
    fr *= 2000.f;
    fr *= dir;
    mesh_->AddImpulseAtLocation(
        fr, mesh_->Constraints[DrivePower_FR]->GetConstraintLocation());
}

// Called to bind functionality to input
void ABEAMBotsToycar::SetupPlayerInputComponent(class UInputComponent* pic)
{
    Super::SetupPlayerInputComponent(pic);

    pic->BindAxis("MoveForward", this, &ABEAMBotsToycar::on_move_forward);
    pic->BindAxis("MoveRight", this, &ABEAMBotsToycar::on_move_right);
}

// Converts input from player controller to physical angular velocity for
// applying to wheels.
static float input_v_to_angular_velocity(float v) {
    const double ROT_VELOCITY = 90.0f;
    return (FMath::Abs(v) <= 0.01f)
        ? 0.0f
        : (v > 0.0f ? ROT_VELOCITY : -ROT_VELOCITY);
}

static FVector get_power_vector(float power) {
    return FVector(0.f, power, 0.f);
}

void ABEAMBotsToycar::on_move_forward(float v) {
    float powr = input_v_to_angular_velocity(v);
    wake_flag_ |= FMath::Abs(v) > 0.1f;
    drive_power_[DrivePower_FL] = drive_power_[DrivePower_BL] = get_power_vector(powr);
    drive_power_[DrivePower_FR] = drive_power_[DrivePower_BR] = get_power_vector(powr);
}

void ABEAMBotsToycar::on_move_right(float v) {
    float powr = input_v_to_angular_velocity(v) * 1.2f;
    if (FMath::Abs(v) > 0.1f) {
        wake_flag_ |= true;
        drive_power_[DrivePower_FL] = drive_power_[DrivePower_BL] = get_power_vector(powr);
        drive_power_[DrivePower_FR] = drive_power_[DrivePower_BR] = get_power_vector(-powr);
    }
}
