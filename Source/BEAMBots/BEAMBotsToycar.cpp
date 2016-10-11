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
    static ConstructorHelpers::FObjectFinder<UMaterial> material_helper(
        TEXT("Material'/Game/Materials/CarPaint/M_CarPaint'"));
    mesh_->SetMaterial(0, material_helper.Object);

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

    if (drive_power_[0].SizeSquared() > 1.0f) {
        mesh_->WakeAllRigidBodies();
    }
    for (auto i = 0; i < mesh_->Constraints.Num(); ++i) {
        mesh_->Constraints[i]->SetAngularVelocityTarget(drive_power_[i]);
    }
//    FVector fl = drive_power_fl_;
//    fl.Normalize();
//    fl *= 1000.f;
//    mesh_->AddImpulseAtLocation(drive_power_fl_,
//                                mesh_->Constraints[2]->GetConstraintLocation());
//
//    FVector fr = drive_power_fr_;
//    fr.Normalize();
//    fr *= 1000.f;
//    mesh_->AddImpulseAtLocation(fr,
//                                mesh_->Constraints[3]->GetConstraintLocation());
}

// Called to bind functionality to input
void ABEAMBotsToycar::SetupPlayerInputComponent(class UInputComponent* pic)
{
    Super::SetupPlayerInputComponent(pic);

    pic->BindAxis("MoveForward", this, &ABEAMBotsToycar::on_move_forward);
    pic->BindAxis("MoveRight", this, &ABEAMBotsToycar::on_move_right);
}

void ABEAMBotsToycar::on_move_forward(float v) {
    const double STRENGTH = 1500.0f; // positive strength rotates backwards
    float powr = (FMath::Abs(v) <= 0.01f)
                 ? 0.0f
                 : (v > 0.0f ? STRENGTH : -STRENGTH);
    for (auto i = 0; i < mesh_->Constraints.Num(); ++i) {
        // Crutch for BL wheel being oriented against the others
        float sign1 = (i == 1) ? 1.0f : -1.0f;
        drive_power_[i] = FVector(sign1 * powr, 0.f, 0.f);
    }
}

void ABEAMBotsToycar::on_move_right(float v) {
}
