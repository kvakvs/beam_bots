// Made by: Dmytro 'kvakvs' Lytovchenko

#include "BEAMBots.h"
#include "BEAMBotsToycar.h"
#include "Engine.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"

// #define OBSERVER_CAMERA_NAME "TopCamera"
#define OBSERVER_CAMERA_NAME "TopCloseupCamera"
const ECollisionChannel COLL_CHANNEL_WHEEL = ECollisionChannel::ECC_GameTraceChannel1;
const ECollisionChannel COLL_CHANNEL_CAR = ECollisionChannel::ECC_GameTraceChannel2;
const char *SOCKET_FR = "SocketFR";
const char *SOCKET_FL = "SocketFL";
const char *SOCKET_BR = "SocketBR";
const char *SOCKET_BL = "SocketBL";

// Sets default values
ABEAMBotsToycar::ABEAMBotsToycar()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    construct_geometry();
    construct_physics();
}

void ABEAMBotsToycar::setup_mesh_component_as_phys(UStaticMeshComponent *comp) {
    comp->BodyInstance.bStartAwake = true;
    comp->SetNotifyRigidBodyCollision(true);
    comp->SetSimulatePhysics(true);
    comp->SetCanEverAffectNavigation(false);
    comp->SetEnableGravity(true);
//    comp->BodyInstance.bUseCCD = true;
//    comp->bGenerateOverlapEvents = true;
}

void ABEAMBotsToycar::construct_geometry() {
    car_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>("Body");
    RootComponent = car_mesh_;

    // Car mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> car_helper(TEXT("/Game/ToyCars/ToyCar1"));
    car_mesh_->SetStaticMesh(car_helper.Object);
    car_mesh_->SetMobility(EComponentMobility::Movable);

    // Car mesh - Collision
//    car_mesh_->SetCollisionObjectType(COLL_CHANNEL_CAR);
    FCollisionResponseContainer crc(    ECollisionResponse::ECR_Block);
    crc.SetResponse(COLL_CHANNEL_WHEEL, ECollisionResponse::ECR_Ignore);
    car_mesh_->SetCollisionResponseToChannels(crc);
    
    // Car mesh - Physics
    setup_mesh_component_as_phys(car_mesh_);
    car_mesh_->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> wheel_helper(TEXT("/Game/ToyCars/ToyWheel"));
    wheel_mesh_ = wheel_helper.Object;
}

void ABEAMBotsToycar::spawn_wheels() {
    ensure(!wheel_fr_);
    ensure(!wheel_fl_);
    ensure(!wheel_br_);
    ensure(!wheel_bl_);
    wheel_fr_ = spawn_wheel("WheelFR", SOCKET_FR);
    wheel_fl_ = spawn_wheel("WheelFL", SOCKET_FL);
    wheel_br_ = spawn_wheel("WheelBR", SOCKET_BR);
    wheel_bl_ = spawn_wheel("WheelBL", SOCKET_BL);
}

AStaticMeshActor *ABEAMBotsToycar::spawn_wheel(
    const FString &name,
    const FString &socket_name) 
{
    auto w = GetWorld();
    ensure(w);
    
    // Spawn and set label
    FActorSpawnParameters spawn;
    spawn.Name = FName(*name);
    spawn.Owner = this;
    
    auto wheel = w->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), spawn);
    ensure(wheel);
    wheel->SetActorLabel(name, true);
    
    // Wheel - Set Mesh and mobility
    auto meshc = wheel->GetStaticMeshComponent();
    meshc->SetMobility(EComponentMobility::Movable);
    meshc->SetStaticMesh(wheel_mesh_);
    
    // Wheel - Collision Query/Response
    meshc->SetCollisionObjectType(COLL_CHANNEL_WHEEL);
    FCollisionResponseContainer crc(    ECollisionResponse::ECR_Block);
    crc.SetResponse(COLL_CHANNEL_WHEEL, ECollisionResponse::ECR_Ignore);
    crc.SetResponse(COLL_CHANNEL_CAR,   ECollisionResponse::ECR_Ignore);
    meshc->SetCollisionResponseToChannels(crc);

    wheel->SetActorLocation(this->GetActorLocation() +
                            car_mesh_->GetSocketLocation(FName(*socket_name)));
    wheel->AttachToActor(this,
                         FAttachmentTransformRules::SnapToTargetIncludingScale,
                         FName(*socket_name));

    setup_mesh_component_as_phys(meshc);

    //UE_LOG(LogText, Warning, "Spawn wheel actor=%p", wheel);
    return wheel;
}

void ABEAMBotsToycar::construct_physics() {
    ensure(!phys_wheel_fr_);
    ensure(!phys_wheel_fl_);
    ensure(!phys_wheel_br_);
    ensure(!phys_wheel_bl_);
    phys_wheel_fr_ = construct_wheel_constraint("PC_WheelFR");
    phys_wheel_fl_ = construct_wheel_constraint("PC_WheelFL");
    phys_wheel_br_ = construct_wheel_constraint("PC_WheelBR");
    phys_wheel_bl_ = construct_wheel_constraint("PC_WheelBL");    
}

UPhysicsConstraintComponent * 
ABEAMBotsToycar::construct_wheel_constraint(const FString & comp_name)
{
    auto phys = this->CreateDefaultSubobject<UPhysicsConstraintComponent>(FName(*comp_name));
//    phys->SetAngularVelocityDrive(false, true);
//    phys->SetAngularVelocityTarget(FVector(0.0f, 0.0f, 0.0f));
//    phys->SetAngularDriveParams(5000.0f, 5000.0f, 5000.0f);
    
    phys->SetLinearVelocityDrive(false, true, false);
    phys->SetLinearDriveParams(5000.0f, 5000.0f, 5000.0f);
    phys->SetLinearVelocityTarget(FVector(0.0f, 0.0f, 0.0f));
    return phys;
}

void ABEAMBotsToycar::setup_physics_motor_wheel(UPhysicsConstraintComponent *phys)
{
//    auto &ad = phys->ConstraintInstance.ProfileInstance.AngularDrive;
//    ad.AngularDriveMode = EAngularDriveMode::TwistAndSwing;
//    ad.TwistDrive.bEnableVelocityDrive = true;
}

void ABEAMBotsToycar::setup_physics_on_play()
{
    ensure(phys_wheel_fr_);ensure(phys_wheel_fl_);ensure(phys_wheel_br_);
    ensure(phys_wheel_bl_);ensure(wheel_fr_);ensure(wheel_fl_);ensure(wheel_br_);
    ensure(wheel_bl_);

    setup_physics_constraint(phys_wheel_fr_, wheel_fr_, SOCKET_FR);
    setup_physics_constraint(phys_wheel_fl_, wheel_fl_, SOCKET_FL);
    setup_physics_motor_wheel(phys_wheel_fr_);
    setup_physics_motor_wheel(phys_wheel_fl_);

    setup_physics_constraint(phys_wheel_br_, wheel_br_, SOCKET_BR);
    setup_physics_constraint(phys_wheel_bl_, wheel_bl_, SOCKET_BL);
}

void ABEAMBotsToycar::setup_physics_constraint(UPhysicsConstraintComponent *phys,
                                               AActor *wheel,
                                               const FString &socket_name)
{
    phys->SetRelativeLocation(car_mesh_->GetSocketLocation(FName(*socket_name)));
    
    phys->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.0f);
    phys->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Free, 0.0f);
    phys->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.0f);

    phys->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
    phys->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);
    phys->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);

    phys->ConstraintActor1 = this;
    phys->ConstraintActor2 = wheel;
    //phys->SetDisableCollision(true);
}

// Called when the game starts or when spawned
void ABEAMBotsToycar::BeginPlay()
{
    spawn_wheels();
    setup_physics_on_play();

    // Setup camera
    for (TActorIterator<ACameraActor> iter(GetWorld()); iter; ++iter) {
        if (iter->GetName() == OBSERVER_CAMERA_NAME) {
            auto pc = GetWorld()->GetFirstPlayerController();
            //auto pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
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
    const double STRENGTH = 50.0f;
    double powr = v == 0.0f ? 0.0f : (v > 0.0f ? STRENGTH : -STRENGTH);

//    phys_wheel_fr_->SetAngularVelocityTarget(FVector(0.0, 0.0, powr));
//    phys_wheel_fl_->SetAngularVelocityTarget(FVector(0.0, 0.0, powr));
    phys_wheel_fl_->SetLinearVelocityTarget(FVector(powr, 0.0f, 0.0f));
    phys_wheel_fr_->SetLinearVelocityTarget(FVector(powr, 0.0f, 0.0f));
}

void ABEAMBotsToycar::on_move_right(float v) {
}
