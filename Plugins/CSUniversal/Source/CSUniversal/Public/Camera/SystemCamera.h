#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobalCameraState.h"
#include "SystemCamera.generated.h"

class UCameraComponent;
class USceneComponent;
class UCameraStateComponent;
class UCameraTargetComponent;
class UCameraPositionComponent;
class UCameraCollisionComponent;
class UCameraRotationComponent;
class UCameraTransitionComponent;
class UFirstPersonCameraComponent;
class UOrbitCameraComponent;

class ASystemCameraEventer; 

UCLASS()
class CSUNIVERSAL_API ASystemCamera : public AActor
{
	GENERATED_BODY()

private:
	void SetTransitionMode(EGlobalCameraTransitionMode NewMode);
	void UpdateBlend(float DeltaTime);
	void UpdateCameraTransform(float DeltaTime);
	void UpdateDiscreteSettings();
	void PushStateToComponents();
	void PullStateFromComponents();

	AActor* GetPositionTarget(EGlobalCameraPositionMode PositionMode) const;
	AActor* GetFollowTarget() const;
	AActor* GetRotationTarget() const;

	void ConvertPositionSpace(EGlobalCameraPositionMode NewPositionMode);
	
	ASystemCameraEventer* LastTgiggeredEventer = nullptr;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<UCameraStateComponent> StateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<UCameraTargetComponent> TargetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<UCameraPositionComponent> PositionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<UCameraCollisionComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<UCameraRotationComponent> RotationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<UCameraTransitionComponent> TransitionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<UFirstPersonCameraComponent> FirstPersonComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<UOrbitCameraComponent> OrbitComponent;

	FVector GetWorldPositionFromState(const FGlobalCameraState& State) const;
	FVector GetRelativePositionFromWorld(const FVector& WorldPosition, EGlobalCameraPositionMode PositionMode) const;

public:
	ASystemCamera();

	virtual void Tick(float DeltaTime) override;

	// Legacy state properties kept for existing Blueprint access.
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
	
	EGlobalCameraControlMode GetCameraControlMode() const { return CurrentState.Control.ControlMode; }

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

	UFUNCTION(BlueprintCallable, Category = "Camera|Mouse")
	void SetTargetInvertThirdPersonPitch(bool bInvert, EGlobalCameraTransitionMode NewMode = EGlobalCameraTransitionMode::Smooth);

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
	
	UFUNCTION(BlueprintCallable, Category = "Camera | Eventer")
	ASystemCameraEventer* GetLastTriggeredEventer() const { return LastTgiggeredEventer; }
	UFUNCTION(BlueprintCallable, Category = "Camera | Eventer")
	void SetLastTriggeredEventer(ASystemCameraEventer* NewEventer) { LastTgiggeredEventer = NewEventer; };
	
};