// Made by: Dmytro 'kvakvs' Lytovchenko

#include "BEAMBots.h"
#include "BEAMBotsToycar.h"
#include "Engine.h"

#include "GenericPlatformMath.h"

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
    
    // Skeleton
//    static ConstructorHelpers::FObjectFinder<USkeletalMesh> skel_helper(
//        TEXT("/Game/ToyCars/ToyCar1_Skeleton"));
//    mesh_->SetSkeletalMesh(skel_helper.Object);

    // Physics Asset
//    static ConstructorHelpers::FObjectFinder<UPhysicsAsset> phys_asset_helper(
//        TEXT("/Game/ToyCars/ToyCar1_PhysicsAsset"));
//    mesh_->SetPhysicsAsset(phys_asset_helper.Object);

    mesh_->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    mesh_->SetSimulatePhysics(true);
    
    // Color+material
    static ConstructorHelpers::FObjectFinder<UMaterial> material_helper(
        TEXT("Material'/Game/Materials/CarPaint/M_CarPaint'"));
    mesh_->SetMaterial(0, material_helper.Object);

//    UPhysicsAsset *pa = mesh_->GetPhysicsAsset(); check(pa);
//    for (auto &n: pa->BodySetupIndexMap) {
//        //FPlatformMisc::LocalPrint("%s\n", n.Key);
//    }
    SetRootComponent(mesh_);
}

// Called when the game starts or when spawned
void ABEAMBotsToycar::BeginPlay()
{
    // Physics components of the car
//    FBodyInstance *body = mesh_->GetBodyInstance("CarBody");
//    check(body);
//    body->SetMassOverride(500.0f);
    
    phys_wheel_fr_ = mesh_->GetBodyInstance("WheelFR");
    check(phys_wheel_fr_);
    
    phys_wheel_fl_ = mesh_->GetBodyInstance("WheelFL");
    check(phys_wheel_fl_);
  
    // Setup camera
    auto w = this->GetWorld();
    for (TActorIterator<ACameraActor> iter(w); iter; ++iter) {
        if (iter->GetName() == OBSERVER_CAMERA_NAME) {
            auto pc = w->GetFirstPlayerController();
            pc->SetViewTarget(*iter, FViewTargetTransitionParams());
            break;
        }
    }

    Super::BeginPlay();
}

// Called every frame
void ABEAMBotsToycar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABEAMBotsToycar::SetupPlayerInputComponent(class UInputComponent* pic)
{
    Super::SetupPlayerInputComponent(pic);

    pic->BindAxis("MoveForward", this, &ABEAMBotsToycar::on_move_forward);
    pic->BindAxis("MoveRight", this, &ABEAMBotsToycar::on_move_right);
}

void ABEAMBotsToycar::on_move_forward(float v) {
    const double STRENGTH = -50.0f; // positive strength rotates backwards
    double powr = (FMath::Abs(v) <= 0.01f)
                  ? 0.0f
                  : (v > 0.0f ? STRENGTH : -STRENGTH);
    FVector pow_vec(-powr, 0, 0);

    check(phys_wheel_fr_);
    phys_wheel_fr_->SetAngularVelocity(pow_vec, false);
    check(phys_wheel_fl_);
    phys_wheel_fl_->SetAngularVelocity(pow_vec, false);
 
}

void ABEAMBotsToycar::on_move_right(float v) {
}
