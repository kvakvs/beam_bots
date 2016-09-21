// Made by: Dmytro 'kvakvs' Lytovchenko

#include "BEAMBots.h"
#include "BrickPawn.h"


// Sets default values
ABrickPawn::ABrickPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    init_geometry();
}

// Called when the game starts or when spawned
void ABrickPawn::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void ABrickPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABrickPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);

}

void ABrickPawn::init_geometry()
{
    //for (TActorIterator<ACameraActor> iter(GetWorld()); iter; ++iter) {
    //    if (iter->GetName() == "TopCamera") {
    //        auto pc = GetWorld()->GetFirstPlayerController();
    //        //auto pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    //        pc->SetViewTarget(*iter, FViewTargetTransitionParams());
    //        break;
    //    }
    //}

    // Car mesh
    //static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(
    //    TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
    //GetMesh()->SetSkeletalMesh(CarMesh.Object);

    //static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(
    //    TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
    //GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

    // Simulation
    //UWheeledVehicleMovementComponent4W* Vehicle4W
    //    = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

    //check(Vehicle4W->WheelSetups.Num() == 4);

    //Vehicle4W->WheelSetups[0].WheelClass = UBEAMBotsWheelFront::StaticClass();
    //Vehicle4W->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
    //Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -12.f, 0.f);

    //Vehicle4W->WheelSetups[1].WheelClass = UBEAMBotsWheelFront::StaticClass();
    //Vehicle4W->WheelSetups[1].BoneName = FName("Wheel_Front_Right");
    //Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 12.f, 0.f);

    //Vehicle4W->WheelSetups[2].WheelClass = UBEAMBotsWheelRear::StaticClass();
    //Vehicle4W->WheelSetups[2].BoneName = FName("Wheel_Rear_Left");
    //Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -12.f, 0.f);

    //Vehicle4W->WheelSetups[3].WheelClass = UBEAMBotsWheelRear::StaticClass();
    //Vehicle4W->WheelSetups[3].BoneName = FName("Wheel_Rear_Right");
    //Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 12.f, 0.f);

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

}

