#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlobalCameraState.h"
#include "CameraTransitionComponent.generated.h"

UCLASS(ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class CSUNIVERSAL_API UCameraTransitionComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	float CurrentRotationSpeed = 1.f;

	void UpdatePositionState(
		FGlobalCameraState& CurrentState,
		const FGlobalCameraState& TargetState,
		EGlobalCameraTransitionMode TransitionMode,
		float DeltaTime
	);

	void UpdateRotationState(
		FGlobalCameraState& CurrentState,
		const FGlobalCameraState& TargetState,
		EGlobalCameraTransitionMode TransitionMode,
		float DeltaTime
	);

	void UpdateCameraSettings(
		FGlobalCameraState& CurrentState,
		const FGlobalCameraState& TargetState,
		EGlobalCameraTransitionMode TransitionMode,
		float DeltaTime
	);

	void UpdateControlSettings(
		FGlobalCameraState& CurrentState,
		const FGlobalCameraState& TargetState,
		EGlobalCameraTransitionMode TransitionMode,
		float DeltaTime
	);

public:
	UCameraTransitionComponent();

	void UpdateTransition(
		FGlobalCameraState& CurrentState,
		const FGlobalCameraState& TargetState,
		EGlobalCameraTransitionMode TransitionMode,
		float DeltaTime
	);

	void ResetRotationSpeed(float NewSpeed = 1.f);

	float GetCurrentRotationSpeed() const;
};