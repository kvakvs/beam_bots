// Made by: Dmytro 'kvakvs' Lytovchenko

#pragma once

#include "GameFramework/Pawn.h"
#include "BEAMBotsToycar.generated.h"

UCLASS()
class BEAMBOTS_API ABEAMBotsToycar : public APawn
{
	GENERATED_BODY()

    UStaticMeshComponent *car_mesh_;
    UStaticMeshComponent *wheel_fr_;
    UStaticMeshComponent *wheel_fl_;
    UStaticMeshComponent *wheel_br_;
    UStaticMeshComponent *wheel_bl_;
public:
	// Sets default values for this pawn's properties
	ABEAMBotsToycar();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void on_move_forward(float v);
	void on_move_right(float v);
    
private:
    void init_geometry();
};
