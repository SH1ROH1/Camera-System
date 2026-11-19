#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlobalCameraState.h"

#include "CameraStateComponent.generated.h"

UCLASS(ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class CSUNIVERSAL_API UCameraStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraStateComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|State")
	FGlobalCameraState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|State")
	FGlobalCameraState TargetState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|State")
	EGlobalCameraTransitionMode TransitionMode = EGlobalCameraTransitionMode::Smooth;

	void SetTargetState(
		const FGlobalCameraState& NewState,
		EGlobalCameraTransitionMode NewTransitionMode
	);

	void SetTransitionMode(EGlobalCameraTransitionMode NewMode);

	void UpdateDiscreteSettings();

	UFUNCTION(BlueprintPure, Category = "Camera|State")
	FGlobalCameraState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Camera|State")
	FGlobalCameraState GetTargetState() const { return TargetState; }

	UFUNCTION(BlueprintPure, Category = "Camera|State")
	EGlobalCameraTransitionMode GetTransitionMode() const { return TransitionMode; }
};