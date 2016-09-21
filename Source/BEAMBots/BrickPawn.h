// Made by: Dmytro 'kvakvs' Lytovchenko
// Implements simple box vehicle and simulates certain popular plastic brick 
// constructor, created in Denmark

#pragma once

#include "GameFramework/Pawn.h"
#include "BrickPawn.generated.h"

UCLASS()
class BEAMBOTS_API ABrickPawn : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    ABrickPawn();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaSeconds) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

private:
    void init_geometry();
};
