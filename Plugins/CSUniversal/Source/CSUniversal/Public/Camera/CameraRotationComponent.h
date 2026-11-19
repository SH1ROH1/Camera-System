
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlobalCameraState.h"
#include "CameraRotationComponent.generated.h"

class AActor;

UCLASS(ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class CSUNIVERSAL_API UCameraRotationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraRotationComponent();

	FRotator GetTargetRotation(
		const FGlobalCameraState& State,
		AActor* FollowTarget
	) const;

	FRotator CalculateDesiredRotation(
		const FGlobalCameraState& State,
		const FVector& CameraLocation,
		AActor* FollowTarget,
		AActor* PlayerTarget,
		AActor* ConnectTarget,
		bool& bShouldUpdateStateRotation
	) const;

	FVector GetLookAtLocation(
		const FGlobalCameraState& State,
		AActor* FollowTarget,
		AActor* PlayerTarget,
		AActor* ConnectTarget,
		bool& bFoundLocation
	) const;
};
