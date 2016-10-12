// Made by: Dmytro 'kvakvs' Lytovchenko

#pragma once

#include "GameFramework/Pawn.h"
//#include "SkeletalMeshActor.h"

class UPhysicsConstraintComponent;
class UCameraComponent;
class USpringArmComponent;

#include "BEAMBotsToycar.generated.h"

UCLASS()
class BEAMBOTS_API ABEAMBotsToycar : public APawn
{
    GENERATED_BODY()

public:
    UPROPERTY(Category=Car, VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    USkeletalMeshComponent *mesh_;

    UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* spring_arm_;
    
    // Camera component that will be our viewpoint
    UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* cam_;
    
    // Controls affect power on the engines
    int motor_index_FR = 0;
    int motor_index_FL = 0;
    int motor_index_BR = 0;
    int motor_index_BL = 0;
    float forward_ = 0.f;
    float steer_ = 0.f;
    
public:
    // Sets default values for this pawn's properties
    ABEAMBotsToycar();


    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaSeconds) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

    void on_move_forward(float v);
    void on_move_right(float v);

private:
    void construct_geometry();
    void power_motor(int index, float power);
    void power_the_motors();
};
