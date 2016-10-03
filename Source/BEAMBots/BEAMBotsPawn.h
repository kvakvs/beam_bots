// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/WheeledVehicle.h"
#include "BEAMBotsPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UTextRenderComponent;
class UInputComponent;
UCLASS(config = Game)
class ABEAMBotsPawn : public AWheeledVehicle
{
    GENERATED_BODY()

    /** Spring arm that will offset the camera */
    //UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    //    USpringArmComponent* spring_arm_;

    /** Camera component that will be our viewpoint */
    //UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    //    UCameraComponent* cam_;

    /** SCene component for the In-Car view origin */
    //UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    //    class USceneComponent* internal_cam_base_;

    /** Camera component for the In-Car view */
    //UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    //    UCameraComponent* internal_cam_;

    /** Text component for the In-Car speed */
    //UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    //    UTextRenderComponent* incar_speed_;

    /** Text component for the In-Car gear */
    //UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    //    UTextRenderComponent* incar_gear_;


public:
    ABEAMBotsPawn();

    /** The current speed as a string eg 10 km/h */
    UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
        FText speed_disp_str_;

    /** The current gear as a string (R,N, 1,2 etc) */
    UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
        FText gear_disp_str_;

    UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
        /** The color of the incar gear text in forward gears */
        FColor gear_disp_color_;

    /** The color of the incar gear text when in reverse */
    UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
        FColor gear_disp_reverse_color_;

    /** Are we using incar camera */
    UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
        bool incar_camera_active_;

    /** Are we in reverse gear */
    UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
        bool is_reverse_gear_;

    /** Initial offset of incar camera */
    FVector internal_camera_org_;
    // Begin Pawn interface
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
    // End Pawn interface

    // Begin Actor interface
    virtual void Tick(float Delta) override;
    virtual void BeginPlay() override;
    // End Actor interface

    /** Handle pressing forwards */
    void MoveForward(float Val);
    /** Setup the strings used on the hud */
    void SetupInCarHUD();

    /** Update the physics material used by the vehicle mesh */
    void UpdatePhysicsMaterial();
    /** Handle pressing right */
    void MoveRight(float Val);
    /** Handle handbrake pressed */
    void OnHandbrakePressed();
    /** Handle handbrake released */
    void OnHandbrakeReleased();
    /** Switch between cameras */
    void OnToggleCamera();
    /** Handle reset VR device */
    void OnResetVR();

    static const FName LookUpBinding;
    static const FName LookRightBinding;

private:
    /**
     * Activate In-Car camera. Enable camera and sets visibility of incar hud display
     *
     * @param	bState true will enable in car view and set visibility of various if its doesnt match new state
     * @param	bForce true will force to always change state
     */
    void EnableIncarView(const bool bState, const bool bForce = false);

    /** Update the gear and speed strings */
    void UpdateHUDStrings();

    /* Are we on a 'slippery' surface (low friction) */
    bool is_slippery_;


public:
    ///** Returns SpringArm subobject **/
    //FORCEINLINE USpringArmComponent* GetSpringArm() const { return spring_arm_; }
    ///** Returns Camera subobject **/
    //FORCEINLINE UCameraComponent* GetCamera() const { return cam_; }
    ///** Returns InternalCamera subobject **/
    //FORCEINLINE UCameraComponent* GetInternalCamera() const { return internal_cam_; }
    ///** Returns InCarSpeed subobject **/
    //FORCEINLINE UTextRenderComponent* GetInCarSpeed() const { return incar_speed_; }
    ///** Returns InCarGear subobject **/
    //FORCEINLINE UTextRenderComponent* GetInCarGear() const { return incar_gear_; }
};
