// Made by: Dmytro 'kvakvs' Lytovchenko

#include "BEAMBots.h"
#include "BEAMBotsObserverPawn.h"

#define OBSERVER_CAMERA_NAME "TopCamera"
//#define OBSERVER_CAMERA_NAME "TopCloseupCamera"

// Sets default values
ABEAMBotsObserverPawn::ABEAMBotsObserverPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABEAMBotsObserverPawn::BeginPlay()
{
    Super::BeginPlay();

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
void ABEAMBotsObserverPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABEAMBotsObserverPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);
}
