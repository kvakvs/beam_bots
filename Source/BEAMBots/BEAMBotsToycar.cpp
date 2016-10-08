// Made by: Dmytro 'kvakvs' Lytovchenko

#include "BEAMBots.h"
#include "BEAMBotsToycar.h"
#include "Engine.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"

// #define OBSERVER_CAMERA_NAME "TopCamera"
#define OBSERVER_CAMERA_NAME "TopCloseupCamera"

// Sets default values
ABEAMBotsToycar::ABEAMBotsToycar()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    construct_geometry();
    construct_physics();
}

void ABEAMBotsToycar::construct_geometry() {
    car_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>("Body");
    car_mesh_->SetCollisionProfileName(TEXT("CollideChannelCar"));
    car_mesh_->BodyInstance.bSimulatePhysics = true;
    car_mesh_->BodyInstance.bNotifyRigidBodyCollision = true;
    car_mesh_->BodyInstance.bUseCCD = true;
    car_mesh_->bGenerateOverlapEvents = true;
    car_mesh_->SetCanEverAffectNavigation(false);
    car_mesh_->SetWorldScale3D(FVector(2.0, 2.0, 2.0));
    RootComponent = car_mesh_;

    // Car mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> car_helper(TEXT("/Game/ToyCars/ToyCar1"));
    car_mesh_->SetStaticMesh(car_helper.Object);
    car_mesh_->SetMobility(EComponentMobility::Movable);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> wheel_helper(TEXT("/Game/ToyCars/ToyWheel"));
    wheel_mesh_ = wheel_helper.Object;
}

void ABEAMBotsToycar::spawn_wheels() {
    ensure(!wheel_fr_);
    ensure(!wheel_fl_);
    ensure(!wheel_br_);
    ensure(!wheel_bl_);
    wheel_fr_ = spawn_wheel("WheelFR", "FrontR");
    wheel_fl_ = spawn_wheel("WheelFL", "FrontL");
    wheel_br_ = spawn_wheel("WheelBR", "BackR");
    wheel_bl_ = spawn_wheel("WheelBL", "BackL");
}

AStaticMeshActor *ABEAMBotsToycar::spawn_wheel(
    const FString &name,
    const FString &socket_name) 
{
    auto w = GetWorld();
    ensure(w);
    
    FActorSpawnParameters spawn;
    spawn.Name = *name;
    spawn.Owner = this;
    auto wheel = w->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), spawn);
    ensure(wheel);
    
    auto meshc = wheel->GetStaticMeshComponent();
    meshc->SetMobility(EComponentMobility::Movable);
    meshc->SetStaticMesh(wheel_mesh_);
    meshc->SetCollisionProfileName(TEXT("CollideChannelWheel"));

    wheel->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(*socket_name));

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
    return phys;
}

void ABEAMBotsToycar::setup_physics_on_play()
{
    ensure(phys_wheel_fr_);
    ensure(phys_wheel_fl_);
    ensure(phys_wheel_br_);
    ensure(phys_wheel_bl_);
    ensure(wheel_fr_);
    ensure(wheel_fl_);
    ensure(wheel_br_);
    ensure(wheel_bl_);
    setup_physics_constraint(phys_wheel_fr_, wheel_fr_);
    setup_physics_constraint(phys_wheel_fl_, wheel_fl_);
    setup_physics_constraint(phys_wheel_br_, wheel_br_);
    setup_physics_constraint(phys_wheel_bl_, wheel_bl_);
}

void ABEAMBotsToycar::setup_physics_constraint(
    UPhysicsConstraintComponent *phys,
    AStaticMeshActor *wheel) 
{
    phys->ConstraintActor1 = this;
    phys->ConstraintActor2 = wheel;
}

// Called when the game starts or when spawned
void ABEAMBotsToycar::BeginPlay()
{
    Super::BeginPlay();

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
    double powr = v == 0.0 ? 0.0 : (v > 0 ? 10.0 : -10.0);

    TArray<UActorComponent*> comps;
    GetComponents(comps);

    if (powr) { UE_LOG(LogTemp, Warning, TEXT("BEAMBots Car: on forward")); }

    for (int i = 0; i < comps.Num(); ++i) {
        auto name = comps[i]->GetName();
        if (name == "PC_WheelFR" || name == "PC_WheelFL") {
            auto pc = Cast<UPhysicsConstraintComponent>(comps[i]);
            pc->SetAngularVelocityTarget(FVector(0.0, powr, 0.0));
        }
    }
}

void ABEAMBotsToycar::on_move_right(float v) {
}
