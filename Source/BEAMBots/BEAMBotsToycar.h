// Made by: Dmytro 'kvakvs' Lytovchenko

#pragma once

#include "GameFramework/Pawn.h"

class UPhysicsConstraintComponent;

#include "BEAMBotsToycar.generated.h"

UCLASS()
class BEAMBOTS_API ABEAMBotsToycar : public APawn
{
    GENERATED_BODY()

    UStaticMeshComponent *car_mesh_;

    UStaticMesh *wheel_mesh_ = nullptr;

    AStaticMeshActor *wheel_fr_ = nullptr;
    AStaticMeshActor *wheel_fl_ = nullptr;
    AStaticMeshActor *wheel_br_ = nullptr;
    AStaticMeshActor *wheel_bl_ = nullptr;

    UPhysicsConstraintComponent *phys_wheel_fr_ = nullptr;
    UPhysicsConstraintComponent *phys_wheel_fl_ = nullptr;
    UPhysicsConstraintComponent *phys_wheel_br_ = nullptr;
    UPhysicsConstraintComponent *phys_wheel_bl_ = nullptr;
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
    void spawn_wheels();
    AStaticMeshActor *spawn_wheel(const FString &name, const FString &socket_name);

    void construct_physics();
    void setup_physics_on_play();
    void setup_physics_constraint(UPhysicsConstraintComponent *phys, AStaticMeshActor *wheel);
    UPhysicsConstraintComponent *construct_wheel_constraint(const FString &comp_name);
};
