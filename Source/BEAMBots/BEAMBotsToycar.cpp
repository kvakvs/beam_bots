// Made by: Dmytro 'kvakvs' Lytovchenko

#include "BEAMBots.h"
#include "BEAMBotsToycar.h"
#include "Engine.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
ABEAMBotsToycar::ABEAMBotsToycar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    init_geometry();
}

void ABEAMBotsToycar::init_geometry() {
    car_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>("Body");
    car_mesh_->SetCollisionProfileName(FTEXT("CollideChannelCar"));
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

    static ConstructorHelpers::FObjectFinder<UStaticMesh> wheel_helper(TEXT("/Game/ToyCars/ToyWheel"));
    wheel_fr_ = CreateDefaultSubobject<UStaticMeshComponent>("WheelFR");
    wheel_fr_->SetStaticMesh(wheel_helper.Object);
    wheel_fr_->SetCollisionProfileName(FTEXT("CollideChannelWheel"));
    wheel_fr_->SetupAttachment(car_mesh_, TEXT("FrontR"));
    
    wheel_fl_ = CreateDefaultSubobject<UStaticMeshComponent>("WheelFL");
    wheel_fl_->SetStaticMesh(wheel_helper.Object);
    wheel_fl_->SetCollisionProfileName(FTEXT("CollideChannelWheel"));
    wheel_fl_->SetupAttachment(car_mesh_, TEXT("FrontL"));
    
    wheel_br_ = CreateDefaultSubobject<UStaticMeshComponent>("WheelBR");
    wheel_br_->SetStaticMesh(wheel_helper.Object);
    wheel_br_->SetCollisionProfileName(FTEXT("CollideChannelWheel"));
    wheel_br_->SetupAttachment(car_mesh_, TEXT("BackR"));
    
    wheel_bl_ = CreateDefaultSubobject<UStaticMeshComponent>("WheelBL");
    wheel_bl_->SetStaticMesh(wheel_helper.Object);
    wheel_bl_->SetCollisionProfileName(FTEXT("CollideChannelWheel"));
    wheel_bl_->SetupAttachment(car_mesh_, TEXT("BackL"));
}

// Called when the game starts or when spawned
void ABEAMBotsToycar::BeginPlay()
{
	Super::BeginPlay();
    
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

// Called every frame
void ABEAMBotsToycar::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

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