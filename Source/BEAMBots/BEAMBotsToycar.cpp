// Made by: Dmytro 'kvakvs' Lytovchenko

#include "BEAMBots.h"
#include "BEAMBotsToycar.h"
#include "Engine.h"

#include "GenericPlatformMath.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"

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

    /*
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
    */
}

// Called when the game starts or when spawned
void ABEAMBotsToycar::BeginPlay()
{
    Super::BeginPlay();

    motor_index_FR = mesh_->FindConstraintIndex("WheelFR");
    motor_index_FL = mesh_->FindConstraintIndex("WheelFL");
    motor_index_BR = mesh_->FindConstraintIndex("WheelBR");
    motor_index_BL = mesh_->FindConstraintIndex("WheelBL");
}

void ABEAMBotsToycar::power_motor(int index, float power) {
    FVector vt(0.f, -power, 0.f);
    mesh_->Constraints[index]->SetAngularVelocityTarget(vt);

//    FString str = FString::Printf(TEXT("%.1f"), power);
//    DrawDebugString(GetWorld(),
//                    mesh_->Constraints[index]->GetConstraintLocation(),
//                    str,
//                    NULL, FColor::White, 0.01f);
    
    if (FMath::Abs(power) > 0.01f) {
        mesh_->WakeAllRigidBodies();
    }
}

// Called every frame
void ABEAMBotsToycar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    power_the_motors();

    DrawDebugString(GetWorld(),
                    this->GetActorLocation(),
                    player_name_,
                    NULL, FColor::White, 0.01f);
}

void ABEAMBotsToycar::power_the_motors() {
    // Flags if physics should wake on drive or steer
//    bool wake_forward = FMath::Abs(forward_) > 0.1f;
//    bool wake_steer = FMath::Abs(steer_) > 0.1f;
//    static const float ANG_VELOCITY = 1.0f;
//    
//    if (wake_forward) {
//        forward_ *= ANG_VELOCITY;
//        power_motor(motor_index_FL, -forward_);
//        power_motor(motor_index_BL, -forward_);
//        power_motor(motor_index_FR, -forward_);
//        power_motor(motor_index_BR, -forward_);
//    } else if (wake_steer) {
//        power_motor(motor_index_FL, -steer_);
//        power_motor(motor_index_BL, -steer_);
//        power_motor(motor_index_FR, steer_);
//        power_motor(motor_index_BR, steer_);
//    } else {
//        // Stop motors
//        power_motor(motor_index_FL, 0.f);
//        power_motor(motor_index_FR, 0.f);
//        power_motor(motor_index_BL, 0.f);
//        power_motor(motor_index_BR, 0.f);
//    }
//
//    if (wake_forward || wake_steer) {
//        mesh_->WakeAllRigidBodies();
//    }
}

// Called to bind functionality to input
void ABEAMBotsToycar::SetupPlayerInputComponent(class UInputComponent* pic)
{
    Super::SetupPlayerInputComponent(pic);

    pic->BindAxis("MoveForward", this, &ABEAMBotsToycar::on_move_forward);
    pic->BindAxis("MoveRight", this, &ABEAMBotsToycar::on_move_right);
}

void ABEAMBotsToycar::on_move_forward(float v) {
    //forward_ = v;
}

void ABEAMBotsToycar::on_move_right(float v) {
    //steer_ = v;
}
