
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobalCameraState.h"
#include "SystemCamera.generated.h"

class UCameraComponent;
class USceneComponent;

UCLASS()
class CSFIRSTPERSON_API ASystemCamera : public AActor
{
	GENERATED_BODY()

private:
	FVector FixedWorldPosition = FVector::ZeroVector;

	void SetTransitionMode(EGlobalCameraTransitionMode NewMode);
	void UpdateBlend(float DeltaTime);
	void UpdatePositionState(float DeltaTime);
	void UpdateRotationState(float DeltaTime);
	void UpdateCameraSettings(float DeltaTime);
	void UpdateControlSettings(float DeltaTime);
	void UpdateDiscreteSettings();
	void UpdateCameraTransform(float DeltaTime);

	AActor* GetPositionTarget(EGlobalCameraPositionMode PositionMode) const;
	AActor* GetFollowTarget() const;
	AActor* GetRotationTarget() const;

	void ConvertPositionSpace(EGlobalCameraPositionMode NewPositionMode);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<USceneComponent> Root;

	FVector GetWorldPositionFromState(const FGlobalCameraState& State) const;
	FVector GetRelativePositionFromWorld(const FVector& WorldPosition, EGlobalCameraPositionMode PositionMode) const;
	FVector CalculateDesiredLocation() const;

public:
	ASystemCamera();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FGlobalCameraState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FGlobalCameraState TargetState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	EGlobalCameraTransitionMode TransitionMode = EGlobalCameraTransitionMode::Smooth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TArray<FGlobalCameraPreset> Presets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	TObjectPtr<AActor> PlayerTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	TObjectPtr<AActor> ConnectTarget;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetCameraState(
		const FGlobalCameraState& NewState,
		EGlobalCameraTransitionMode NewTransitionMode = EGlobalCameraTransitionMode::Smooth
	);

	UFUNCTION(BlueprintPure, Category = "Camera")
	FGlobalCameraState GetState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Camera")
	FGlobalCameraState GetTargetState() const { return TargetState; }

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetPosition(FVector NewPosition, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetPositionMode(EGlobalCameraPositionMode NewMode, EGlobalCameraTransitionMode Transition = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetFollow(EGlobalCameraFollowTarget NewTarget, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetCollision(bool bEnabled, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetRotation(FRotator NewRotation, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetRotationSettings(bool bUseTargetRotation, bool bUsePathRotation, float NewLockOnRotation, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetLookAt(bool bEnabled, EGlobalCameraLookMode NewLookMode, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetFieldOfView(float NewFieldOfView, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetMovementSpeed(float NewSpeed, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetRotationSpeed(float NewSpeed, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetBlendSpeed(float NewSpeed, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetControlMode(EGlobalCameraControlMode NewMode, EGlobalCameraTransitionMode Transition = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetMouseSensitivity(float NewSensitivity, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetOrbitSettings(float NewYaw, float NewPitch, float NewDistance, float NewMinPitch, float NewMaxPitch, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetTargetAimSettings(float NewDistance, float NewHeight, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void UpdateCameraRotation(float LookX, float LookY);

	UFUNCTION(BlueprintCallable, Category = "Target")
	void SetPlayerTarget(AActor* NewTarget);

	UFUNCTION(BlueprintPure, Category = "Target")
	AActor* GetPlayerTarget() const { return PlayerTarget; }

	UFUNCTION(BlueprintCallable, Category = "Target")
	void SetConnectTarget(AActor* NewTarget);

	UFUNCTION(BlueprintPure, Category = "Target")
	AActor* GetConnectTarget() const { return ConnectTarget; }

	AActor* GetTarget() const;
};
